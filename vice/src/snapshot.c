/*
 * snapshot.c - Implementation of machine snapshot files.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "lib.h"
#include "ioutil.h"
#include "log.h"
#include "snapshot.h"
#ifdef USE_SVN_REVISION
#include "svnversion.h"
#endif
#include "types.h"
#include "uiapi.h"
#include "version.h"
#include "vsync.h"
#include "zfile.h"

#ifndef offsetof
#define offsetof(type, member) ((size_t)((char*)&(((type*)0)->member) - (char*)0))
#endif

#ifndef container_of
#define container_of(ptr, type, member) ((type*)((char*)(ptr) - offsetof(type, member)))
#endif

typedef struct snapshot_file_stream_s snapshot_file_stream_t;
typedef struct snapshot_memory_stream_s snapshot_memory_stream_t;

static int snapshot_error = SNAPSHOT_NO_ERROR;
static char *current_module = NULL;
static char read_name[SNAPSHOT_MACHINE_NAME_LEN];
static char *current_machine_name = NULL;
static char *current_filename = NULL;

char snapshot_magic_string[] = "VICE Snapshot File\032";
char snapshot_version_magic_string[] = "VICE Version\032";

#define SNAPSHOT_MAGIC_LEN              19
#define SNAPSHOT_VERSION_MAGIC_LEN      13

/* Stream operations interface */
struct snapshot_stream_ops_s {

    /* Read stream */
    size_t      (*read)(snapshot_stream_t* stream, void* ptr, size_t size);

    /* Write stream */
    size_t      (*write)(snapshot_stream_t* stream, const void* ptr, size_t size);

    /* Get stream position */
    long        (*tell)(snapshot_stream_t* stream);

    /* Set stream position */
    int         (*seek)(snapshot_stream_t* stream, long offset, int whence);

    /* Close stream */
    int         (*close)(snapshot_stream_t* stream);

    /* Close stream and erase file */
    int         (*close_erase)(snapshot_stream_t* stream);

    /* Get filename or counterpart for logging */
    const char* (*filename)(snapshot_stream_t* stream);
};

/* Stream base */
struct snapshot_stream_s {
    /* Operations */
    struct snapshot_stream_ops_s *ops;
};

/* FILE based stream */
struct snapshot_file_stream_s {
    /* Operations interface */
    snapshot_stream_t istream;

    /* File descriptor */
    FILE* file;

    /* File name */
    char* filename;
};

/* Memory based stream */
struct snapshot_memory_stream_s {
    /* Operations interface */
    snapshot_stream_t istream;

    /* Flag: are we writing it?  */
    int write_mode;

    /* Memory buffer */
    uint8_t* buffer;

    /* Memory buffer size */
    size_t buffer_size;

    /* Stream pointer */
    long pointer;

    /* Stream size */
    size_t stream_size;
};

struct snapshot_module_s {
    /* Snapshot stream */
    snapshot_stream_t* file;

    /* Flag: are we writing it?  */
    int write_mode;

    /* Size of the module.  */
    uint32_t size;

    /* Offset of the module in the file.  */
    long offset;

    /* Offset of the size field in the file.  */
    long size_offset;
};

struct snapshot_s {
    /* Snapshot stream */
    snapshot_stream_t* file;

    /* Offset of the first module.  */
    long first_module_offset;

    /* Flag: are we writing it?  */
    int write_mode;
};

/* ------------------------------------------------------------------------- */
/* FILE based stream */

static size_t snapshot_file_read(snapshot_stream_t* f, void* ptr, size_t size)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    return fread(ptr, size, 1, file_stream->file);
}

static size_t snapshot_file_write(snapshot_stream_t* f, const void* ptr, size_t size)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    return fwrite(ptr, size, 1, file_stream->file);
}

static long snapshot_file_ftell(snapshot_stream_t* f)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    return ftell(file_stream->file);
}

static int snapshot_file_fseek(snapshot_stream_t *f, long offset, int whence)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    return fseek(file_stream->file, offset, whence);
}

static int snapshot_file_fclose(snapshot_stream_t *f)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    int res = fclose(file_stream->file);
    lib_free(file_stream->filename);
    lib_free(file_stream);
    return res;
}

static int snapshot_file_fclose_erase(snapshot_stream_t *f)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    int res = fclose(file_stream->file);
    ioutil_remove(file_stream->filename);
    lib_free(file_stream->filename);
    lib_free(file_stream);
    return res;
}

static const char* snapshot_file_filename(snapshot_stream_t *f)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    return file_stream->filename;
}

static int snapshot_zfile_fclose(snapshot_stream_t *f)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    int res = zfile_fclose(file_stream->file);
    lib_free(file_stream->filename);
    lib_free(file_stream);
    return res;
}

static int snapshot_zfile_fclose_erase(snapshot_stream_t *f)
{
    snapshot_file_stream_t* file_stream = container_of(f, snapshot_file_stream_t, istream);
    int res = zfile_fclose(file_stream->file);
    ioutil_remove(file_stream->filename);
    lib_free(file_stream->filename);
    lib_free(file_stream);
    return res;
}

static struct snapshot_stream_ops_s snapshot_file_ops = {
    /* read */ snapshot_file_read,
    /* write */ snapshot_file_write,
    /* tell */ snapshot_file_ftell,
    /* seek */ snapshot_file_fseek,
    /* close */ snapshot_file_fclose,
    /* close_erase */ snapshot_file_fclose_erase,
    /* filename */ snapshot_file_filename
};

snapshot_stream_t* snapshot_file_fopen(const char* pathname, const char* mode)
{
    snapshot_file_stream_t* stream = lib_malloc(sizeof(snapshot_file_stream_t));

    lib_free(current_filename);
    current_filename = lib_stralloc(pathname);

    if (stream == NULL) {
        goto fail;
    }

    stream->filename = lib_stralloc(pathname);
    if (stream->filename == NULL) {
        goto delete_stream;
    }

    stream->file = fopen(pathname, mode);
    if (stream->file == NULL) {
        goto delete_filename;
    }

    stream->istream.ops = &snapshot_file_ops;
    return &stream->istream;

delete_filename:
    lib_free(stream->filename);
delete_stream:
    lib_free(stream);
fail:
    return NULL;
}

snapshot_stream_t* snapshot_file_write_fopen(const char* pathname)
{
    return snapshot_file_fopen(pathname, MODE_WRITE);
}

static struct snapshot_stream_ops_s snapshot_zfile_ops = {
    /* read */ snapshot_file_read,
    /* write */ snapshot_file_write,
    /* tell */ snapshot_file_ftell,
    /* seek */ snapshot_file_fseek,
    /* close */ snapshot_zfile_fclose,
    /* close_erase */ snapshot_zfile_fclose_erase,
    /* filename */ snapshot_file_filename
};

snapshot_stream_t* snapshot_zfile_fopen(const char* pathname, const char* mode)
{
    snapshot_file_stream_t* stream = lib_malloc(sizeof(snapshot_file_stream_t));

    lib_free(current_filename);
    current_filename = lib_stralloc(pathname);

    if (stream == NULL) {
        goto fail;
    }

    stream->filename = lib_stralloc(pathname);
    if (stream->filename == NULL) {
        goto delete_stream;
    }

    stream->file = zfile_fopen(pathname, mode);
    if (stream->file == NULL) {
        goto delete_filename;
    }

    stream->istream.ops = &snapshot_zfile_ops;
    return &stream->istream;

delete_filename:
    lib_free(stream->filename);
delete_stream:
    lib_free(stream);
fail:
    return NULL;
}

snapshot_stream_t* snapshot_file_read_fopen(const char* pathname)
{
    return snapshot_zfile_fopen(pathname, MODE_READ);
}


/* ------------------------------------------------------------------------- */
/* Memory based stream */

static size_t snapshot_memory_read(snapshot_stream_t* f, void* ptr, size_t size)
{
    snapshot_memory_stream_t* stream = container_of(f, snapshot_memory_stream_t, istream);
    long pointer = stream->pointer;
    if (stream->buffer == NULL) {
        return -1;
    }

    if (pointer + size > stream->stream_size) {
        return -1;
    }

    memcpy(ptr, stream->buffer + pointer, size);
    stream->pointer = pointer + size;
    return 1;
}

static size_t snapshot_memory_write(snapshot_stream_t* f, const void* ptr, size_t size)
{
    snapshot_memory_stream_t* stream = container_of(f, snapshot_memory_stream_t, istream);
    long pointer = stream->pointer;
    if (stream->write_mode == 0) {
        return -1;
    }

    if (stream->buffer != NULL) {
        if (pointer + size > stream->buffer_size) {
            return -1;
        }

        memcpy(stream->buffer + pointer, ptr, size);
    }

    stream->pointer = pointer + size;
    if (stream->pointer > stream->stream_size) {
        stream->stream_size = stream->pointer;
    }

    return 1;
}

static long snapshot_memory_ftell(snapshot_stream_t* f)
{
    snapshot_memory_stream_t* stream = container_of(f, snapshot_memory_stream_t, istream);
    return stream->pointer;
}

static int snapshot_memory_fseek(snapshot_stream_t *f, long offset, int whence)
{
    snapshot_memory_stream_t* stream = container_of(f, snapshot_memory_stream_t, istream);
    if (whence == SEEK_SET) {
        stream->pointer = offset;
    } else if (whence == SEEK_CUR) {
        stream->pointer += offset;
    } else if (whence == SEEK_END) {
        stream->pointer = stream->stream_size + offset;
    } else {
        return -1;
    }
    return 0;
}

static int snapshot_memory_fclose(snapshot_stream_t *f)
{
    snapshot_memory_stream_t* stream = container_of(f, snapshot_memory_stream_t, istream);
    lib_free(stream);
    return 0;
}

static const char* snapshot_memory_filename(snapshot_stream_t* f)
{
    return "<memory>";
}

static struct snapshot_stream_ops_s snapshot_memory_ops = {
    /* read */ snapshot_memory_read,
    /* write */ snapshot_memory_write,
    /* tell */ snapshot_memory_ftell,
    /* seek */ snapshot_memory_fseek,
    /* close */ snapshot_memory_fclose,
    /* close_erase */ snapshot_memory_fclose,
    /* filename */ snapshot_memory_filename
};

snapshot_stream_t* snapshot_memory_write_fopen(void* buffer, size_t buffer_size)
{
    snapshot_memory_stream_t* stream = lib_malloc(sizeof(snapshot_memory_stream_t));

    lib_free(current_filename);
    current_filename = lib_stralloc("<memory>");

    if (stream == NULL) {
        goto fail;
    }

    stream->write_mode = 1;
    stream->buffer = (uint8_t*) buffer;
    stream->buffer_size = buffer_size;
    stream->pointer = 0;
    stream->stream_size = 0;
    stream->istream.ops = &snapshot_memory_ops;
    return &stream->istream;

fail:
    return NULL;
}

snapshot_stream_t* snapshot_memory_read_fopen(const void* buffer, size_t buffer_size)
{
    snapshot_memory_stream_t* stream = lib_malloc(sizeof(snapshot_memory_stream_t));
    if (stream == NULL) {
        goto fail;
    }

    stream->write_mode = 0;
    stream->buffer = (uint8_t*) buffer;
    stream->buffer_size = buffer_size;
    stream->pointer = 0;
    stream->stream_size = buffer_size;
    stream->istream.ops = &snapshot_memory_ops;
    return &stream->istream;

fail:
    return NULL;
}


/* ------------------------------------------------------------------------- */

size_t snapshot_read(snapshot_stream_t* f, void* ptr, size_t size)
{
    return f->ops->read(f, ptr, size);
}

size_t snapshot_write(snapshot_stream_t* f, const void* ptr, size_t size)
{
    return f->ops->write(f, ptr, size);
}

long snapshot_ftell(snapshot_stream_t *f)
{
    return f->ops->tell(f);
}

int snapshot_fseek(snapshot_stream_t *f, long offset, int whence)
{
    return f->ops->seek(f, offset, whence);
}

int snapshot_fclose(snapshot_stream_t *f)
{
    if (f == NULL)
        return 0;
    return f->ops->close(f);
}

int snapshot_fclose_erase(snapshot_stream_t *f)
{
    if (f == NULL)
        return 0;
    return f->ops->close_erase(f);
}

static const char* snapshot_filename(snapshot_stream_t *f)
{
    return f->ops->filename(f);
}

/* ------------------------------------------------------------------------- */

static int snapshot_write_byte(snapshot_stream_t *f, uint8_t data)
{
    if (snapshot_write(f, &data, 1) != 1) {
        snapshot_error = SNAPSHOT_WRITE_EOF_ERROR;
        return -1;
    }

    return 0;
}

static int snapshot_write_word(snapshot_stream_t *f, uint16_t data)
{
    if (snapshot_write_byte(f, (uint8_t)(data & 0xff)) < 0
        || snapshot_write_byte(f, (uint8_t)(data >> 8)) < 0) {
        return -1;
    }

    return 0;
}

static int snapshot_write_dword(snapshot_stream_t *f, uint32_t data)
{
    if (snapshot_write_word(f, (uint16_t)(data & 0xffff)) < 0
        || snapshot_write_word(f, (uint16_t)(data >> 16)) < 0) {
        return -1;
    }

    return 0;
}

static int snapshot_write_double(snapshot_stream_t *f, double data)
{
    uint8_t *byte_data = (uint8_t *)&data;
    int i;

    for (i = 0; i < sizeof(double); i++) {
        if (snapshot_write_byte(f, byte_data[i]) < 0) {
            return -1;
        }
    }
    return 0;
}

static int snapshot_write_padded_string(snapshot_stream_t *f, const char *s, uint8_t pad_char,
                                        int len)
{
    int i, found_zero;
    uint8_t c;

    for (i = found_zero = 0; i < len; i++) {
        if (!found_zero && s[i] == 0) {
            found_zero = 1;
        }
        c = found_zero ? (uint8_t)pad_char : (uint8_t) s[i];
        if (snapshot_write_byte(f, c) < 0) {
            return -1;
        }
    }

    return 0;
}

static int snapshot_write_byte_array(snapshot_stream_t *f, const uint8_t *data, unsigned int num)
{
    if (num > 0 && snapshot_write(f, data, (size_t)num) != 1) {
        snapshot_error = SNAPSHOT_WRITE_BYTE_ARRAY_ERROR;
        return -1;
    }

    return 0;
}

static int snapshot_write_word_array(snapshot_stream_t *f, const uint16_t *data, unsigned int num)
{
    unsigned int i;

    for (i = 0; i < num; i++) {
        if (snapshot_write_word(f, data[i]) < 0) {
            return -1;
        }
    }

    return 0;
}

static int snapshot_write_dword_array(snapshot_stream_t *f, const uint32_t *data, unsigned int num)
{
    unsigned int i;

    for (i = 0; i < num; i++) {
        if (snapshot_write_dword(f, data[i]) < 0) {
            return -1;
        }
    }

    return 0;
}


static int snapshot_write_string(snapshot_stream_t *f, const char *s)
{
    size_t len, i;

    len = s ? (strlen(s) + 1) : 0;      /* length includes nullbyte */

    if (snapshot_write_word(f, (uint16_t)len) < 0) {
        return -1;
    }

    for (i = 0; i < len; i++) {
        if (snapshot_write_byte(f, s[i]) < 0) {
            return -1;
        }
    }

    return (int)(len + sizeof(uint16_t));
}

static int snapshot_read_byte(snapshot_stream_t *f, uint8_t *b_return)
{
    uint8_t b;

    if (snapshot_read(f, &b, 1) != 1) {
        snapshot_error = SNAPSHOT_READ_EOF_ERROR;
        return -1;
    }
    *b_return = b;
    return 0;
}

static int snapshot_read_word(snapshot_stream_t *f, uint16_t *w_return)
{
    uint8_t lo, hi;

    if (snapshot_read_byte(f, &lo) < 0 || snapshot_read_byte(f, &hi) < 0) {
        return -1;
    }

    *w_return = lo | (hi << 8);
    return 0;
}

static int snapshot_read_dword(snapshot_stream_t *f, uint32_t *dw_return)
{
    uint16_t lo, hi;

    if (snapshot_read_word(f, &lo) < 0 || snapshot_read_word(f, &hi) < 0) {
        return -1;
    }

    *dw_return = lo | (hi << 16);
    return 0;
}

static int snapshot_read_double(snapshot_stream_t *f, double *d_return)
{
    int i;
    uint8_t b;
    double val;
    uint8_t *byte_val = (uint8_t *)&val;

    for (i = 0; i < sizeof(double); i++) {
        if (snapshot_read(f, &b, 1) != 1) {
            snapshot_error = SNAPSHOT_READ_EOF_ERROR;
            return -1;
        }
        byte_val[i] = b;
    }
    *d_return = val;
    return 0;
}

static int snapshot_read_byte_array(snapshot_stream_t *f, uint8_t *b_return, unsigned int num)
{
    if (num > 0 && snapshot_read(f, b_return, (size_t)num) != 1) {
        snapshot_error = SNAPSHOT_READ_BYTE_ARRAY_ERROR;
        return -1;
    }

    return 0;
}

static int snapshot_read_word_array(snapshot_stream_t *f, uint16_t *w_return, unsigned int num)
{
    unsigned int i;

    for (i = 0; i < num; i++) {
        if (snapshot_read_word(f, w_return + i) < 0) {
            return -1;
        }
    }

    return 0;
}

static int snapshot_read_dword_array(snapshot_stream_t *f, uint32_t *dw_return, unsigned int num)
{
    unsigned int i;

    for (i = 0; i < num; i++) {
        if (snapshot_read_dword(f, dw_return + i) < 0) {
            return -1;
        }
    }

    return 0;
}

static int snapshot_read_string(snapshot_stream_t *f, char **s)
{
    int i, len;
    uint16_t w;
    char *p = NULL;

    /* first free the previous string */
    lib_free(*s);
    *s = NULL;      /* don't leave a bogus pointer */

    if (snapshot_read_word(f, &w) < 0) {
        return -1;
    }

    len = (int)w;

    if (len) {
        p = lib_malloc(len);
        *s = p;

        for (i = 0; i < len; i++) {
            if (snapshot_read_byte(f, (uint8_t *)(p + i)) < 0) {
                p[0] = 0;
                return -1;
            }
        }
        p[len - 1] = 0;   /* just to be save */
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

int snapshot_module_write_byte(snapshot_module_t *m, uint8_t b)
{
    if (snapshot_write_byte(m->file, b) < 0) {
        return -1;
    }

    m->size++;
    return 0;
}

int snapshot_module_write_word(snapshot_module_t *m, uint16_t w)
{
    if (snapshot_write_word(m->file, w) < 0) {
        return -1;
    }

    m->size += 2;
    return 0;
}

int snapshot_module_write_dword(snapshot_module_t *m, uint32_t dw)
{
    if (snapshot_write_dword(m->file, dw) < 0) {
        return -1;
    }

    m->size += 4;
    return 0;
}

int snapshot_module_write_double(snapshot_module_t *m, double db)
{
    if (snapshot_write_double(m->file, db) < 0) {
        return -1;
    }

    m->size += 8;
    return 0;
}

int snapshot_module_write_padded_string(snapshot_module_t *m, const char *s, uint8_t pad_char, int len)
{
    if (snapshot_write_padded_string(m->file, s, (uint8_t)pad_char, len) < 0) {
        return -1;
    }

    m->size += len;
    return 0;
}

int snapshot_module_write_byte_array(snapshot_module_t *m, const uint8_t *b, unsigned int num)
{
    if (snapshot_write_byte_array(m->file, b, num) < 0) {
        return -1;
    }

    m->size += num;
    return 0;
}

int snapshot_module_write_word_array(snapshot_module_t *m, const uint16_t *w, unsigned int num)
{
    if (snapshot_write_word_array(m->file, w, num) < 0) {
        return -1;
    }

    m->size += num * sizeof(uint16_t);
    return 0;
}

int snapshot_module_write_dword_array(snapshot_module_t *m, const uint32_t *dw, unsigned int num)
{
    if (snapshot_write_dword_array(m->file, dw, num) < 0) {
        return -1;
    }

    m->size += num * sizeof(uint32_t);
    return 0;
}

int snapshot_module_write_string(snapshot_module_t *m, const char *s)
{
    int len;
    len = snapshot_write_string(m->file, s);
    if (len < 0) {
        snapshot_error = SNAPSHOT_ILLEGAL_STRING_LENGTH_ERROR;
        return -1;
    }

    m->size += len;
    return 0;
}

/* ------------------------------------------------------------------------- */

int snapshot_module_read_byte(snapshot_module_t *m, uint8_t *b_return)
{
    if (snapshot_ftell(m->file) + sizeof(uint8_t) > m->offset + m->size) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_byte(m->file, b_return);
}

int snapshot_module_read_word(snapshot_module_t *m, uint16_t *w_return)
{
    if (snapshot_ftell(m->file) + sizeof(uint16_t) > m->offset + m->size) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_word(m->file, w_return);
}

int snapshot_module_read_dword(snapshot_module_t *m, uint32_t *dw_return)
{
    if (snapshot_ftell(m->file) + sizeof(uint32_t) > m->offset + m->size) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_dword(m->file, dw_return);
}

int snapshot_module_read_double(snapshot_module_t *m, double *db_return)
{
    if (snapshot_ftell(m->file) + sizeof(double) > m->offset + m->size) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_double(m->file, db_return);
}

int snapshot_module_read_byte_array(snapshot_module_t *m, uint8_t *b_return, unsigned int num)
{
    if ((long)(snapshot_ftell(m->file) + num) > (long)(m->offset + m->size)) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_byte_array(m->file, b_return, num);
}

int snapshot_module_read_word_array(snapshot_module_t *m, uint16_t *w_return, unsigned int num)
{
    if ((long)(snapshot_ftell(m->file) + num * sizeof(uint16_t)) > (long)(m->offset + m->size)) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_word_array(m->file, w_return, num);
}

int snapshot_module_read_dword_array(snapshot_module_t *m, uint32_t *dw_return, unsigned int num)
{
    if ((long)(snapshot_ftell(m->file) + num * sizeof(uint32_t)) > (long)(m->offset + m->size)) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_dword_array(m->file, dw_return, num);
}

int snapshot_module_read_string(snapshot_module_t *m, char **charp_return)
{
    if (snapshot_ftell(m->file) + sizeof(uint16_t) > m->offset + m->size) {
        snapshot_error = SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR;
        return -1;
    }

    return snapshot_read_string(m->file, charp_return);
}

int snapshot_module_read_byte_into_int(snapshot_module_t *m, int *value_return)
{
    uint8_t b;

    if (snapshot_module_read_byte(m, &b) < 0) {
        return -1;
    }
    *value_return = (int)b;
    return 0;
}

int snapshot_module_read_word_into_int(snapshot_module_t *m, int *value_return)
{
    uint16_t b;

    if (snapshot_module_read_word(m, &b) < 0) {
        return -1;
    }
    *value_return = (int)b;
    return 0;
}

int snapshot_module_read_dword_into_ulong(snapshot_module_t *m, unsigned long *value_return)
{
    uint32_t b;

    if (snapshot_module_read_dword(m, &b) < 0) {
        return -1;
    }
    *value_return = (unsigned long)b;
    return 0;
}

int snapshot_module_read_dword_into_int(snapshot_module_t *m, int *value_return)
{
    uint32_t b;

    if (snapshot_module_read_dword(m, &b) < 0) {
        return -1;
    }
    *value_return = (int)b;
    return 0;
}

int snapshot_module_read_dword_into_uint(snapshot_module_t *m, unsigned int *value_return)
{
    uint32_t b;

    if (snapshot_module_read_dword(m, &b) < 0) {
        return -1;
    }
    *value_return = (unsigned int)b;
    return 0;
}

/* ------------------------------------------------------------------------- */

snapshot_module_t *snapshot_module_create(snapshot_t *s, const char *name, uint8_t major_version, uint8_t minor_version)
{
    snapshot_module_t *m;

    current_module = (char *)name;

    m = lib_malloc(sizeof(snapshot_module_t));
    m->file = s->file;
    m->offset = snapshot_ftell(s->file);
    if (m->offset == -1) {
        snapshot_error = SNAPSHOT_ILLEGAL_OFFSET_ERROR;
        lib_free(m);
        return NULL;
    }
    m->write_mode = 1;

    if (snapshot_write_padded_string(s->file, name, (uint8_t)0, SNAPSHOT_MODULE_NAME_LEN) < 0
        || snapshot_write_byte(s->file, major_version) < 0
        || snapshot_write_byte(s->file, minor_version) < 0
        || snapshot_write_dword(s->file, 0) < 0) {
        return NULL;
    }

    m->size = snapshot_ftell(s->file) - m->offset;
    m->size_offset = snapshot_ftell(s->file) - sizeof(uint32_t);

    return m;
}

snapshot_module_t *snapshot_module_open(snapshot_t *s, const char *name, uint8_t *major_version_return, uint8_t *minor_version_return)
{
    snapshot_module_t *m;
    char n[SNAPSHOT_MODULE_NAME_LEN];
    unsigned int name_len = (unsigned int)strlen(name);

    current_module = (char *)name;

    if (snapshot_fseek(s->file, s->first_module_offset, SEEK_SET) < 0) {
        snapshot_error = SNAPSHOT_FIRST_MODULE_NOT_FOUND_ERROR;
        return NULL;
    }

    m = lib_malloc(sizeof(snapshot_module_t));
    m->file = s->file;
    m->write_mode = 0;

    m->offset = s->first_module_offset;

    /* Search for the module name.  This is quite inefficient, but I don't
       think we care.  */
    while (1) {
        if (snapshot_read_byte_array(s->file, (uint8_t *)n,
                                     SNAPSHOT_MODULE_NAME_LEN) < 0
            || snapshot_read_byte(s->file, major_version_return) < 0
            || snapshot_read_byte(s->file, minor_version_return) < 0
            || snapshot_read_dword(s->file, &m->size)) {
            snapshot_error = SNAPSHOT_MODULE_HEADER_READ_ERROR;
            goto fail;
        }

        /* Found?  */
        if (memcmp(n, name, name_len) == 0
            && (name_len == SNAPSHOT_MODULE_NAME_LEN || n[name_len] == 0)) {
            break;
        }

        m->offset += m->size;
        if (snapshot_fseek(s->file, m->offset, SEEK_SET) < 0) {
            snapshot_error = SNAPSHOT_MODULE_NOT_FOUND_ERROR;
            goto fail;
        }
    }

    m->size_offset = snapshot_ftell(s->file) - sizeof(uint32_t);

    return m;

fail:
    snapshot_fseek(s->file, s->first_module_offset, SEEK_SET);
    lib_free(m);
    return NULL;
}

int snapshot_module_close(snapshot_module_t *m)
{
    /* Backpatch module size if writing.  */
    if (m->write_mode
        && (snapshot_fseek(m->file, m->size_offset, SEEK_SET) < 0
            || snapshot_write_dword(m->file, m->size) < 0)) {
        snapshot_error = SNAPSHOT_MODULE_CLOSE_ERROR;
        return -1;
    }

    /* Skip module.  */
    if (snapshot_fseek(m->file, m->offset + m->size, SEEK_SET) < 0) {
        snapshot_error = SNAPSHOT_MODULE_SKIP_ERROR;
        return -1;
    }

    lib_free(m);
    return 0;
}

/* ------------------------------------------------------------------------- */

snapshot_t *snapshot_create_from_stream(snapshot_stream_t *f, uint8_t major_version, uint8_t minor_version, const char *snapshot_machine_name)
{
    snapshot_t *s;
    unsigned char viceversion[4] = { VERSION_RC_NUMBER };

    if (f == NULL) {
        snapshot_error = SNAPSHOT_CANNOT_CREATE_SNAPSHOT_ERROR;
        return NULL;
    }

    /* Magic string.  */
    if (snapshot_write_padded_string(f, snapshot_magic_string, (uint8_t)0, SNAPSHOT_MAGIC_LEN) < 0) {
        snapshot_error = SNAPSHOT_CANNOT_WRITE_MAGIC_STRING_ERROR;
        goto fail;
    }

    /* Version number.  */
    if (snapshot_write_byte(f, major_version) < 0
        || snapshot_write_byte(f, minor_version) < 0) {
        snapshot_error = SNAPSHOT_CANNOT_WRITE_VERSION_ERROR;
        goto fail;
    }

    /* Machine.  */
    if (snapshot_write_padded_string(f, snapshot_machine_name, (uint8_t)0, SNAPSHOT_MACHINE_NAME_LEN) < 0) {
        snapshot_error = SNAPSHOT_CANNOT_WRITE_MACHINE_NAME_ERROR;
        goto fail;
    }

    /* VICE version and revision */
    if (snapshot_write_padded_string(f, snapshot_version_magic_string, (uint8_t)0, SNAPSHOT_VERSION_MAGIC_LEN) < 0) {
        snapshot_error = SNAPSHOT_CANNOT_WRITE_MAGIC_STRING_ERROR;
        goto fail;
    }

    if (snapshot_write_byte(f, viceversion[0]) < 0
        || snapshot_write_byte(f, viceversion[1]) < 0
        || snapshot_write_byte(f, viceversion[2]) < 0
        || snapshot_write_byte(f, viceversion[3]) < 0
#ifdef USE_SVN_REVISION
        || snapshot_write_dword(f, VICE_SVN_REV_NUMBER) < 0) {
#else
        || snapshot_write_dword(f, 0) < 0) {
#endif
        snapshot_error = SNAPSHOT_CANNOT_WRITE_VERSION_ERROR;
        goto fail;
    }

    s = lib_malloc(sizeof(snapshot_t));
    s->file = f;
    s->first_module_offset = snapshot_ftell(f);
    s->write_mode = 1;

    return s;

fail:
    return NULL;
}


snapshot_t *snapshot_create(const char *filename, uint8_t major_version, uint8_t minor_version, const char *snapshot_machine_name)
{
    snapshot_stream_t *file;
    snapshot_t *snapshot;

    file = snapshot_file_write_fopen(filename);
    snapshot = snapshot_create_from_stream(file, major_version, minor_version, snapshot_machine_name);
    if (!snapshot) {
        snapshot_fclose_erase(file);
    }
    return snapshot;
}

/* informal only, used by the error message created below */
static unsigned char snapshot_viceversion[4];
static uint32_t snapshot_vicerevision;

snapshot_t *snapshot_open_from_stream(snapshot_stream_t *f, uint8_t *major_version_return, uint8_t *minor_version_return, const char *snapshot_machine_name)
{
    char magic[SNAPSHOT_MAGIC_LEN];
    snapshot_t *s = NULL;
    int machine_name_len;
    size_t offs;

    if (f == NULL) {
        snapshot_error = SNAPSHOT_CANNOT_OPEN_FOR_READ_ERROR;
        return NULL;
    }

    current_machine_name = (char *)snapshot_machine_name;
    current_module = NULL;

    /* Magic string.  */
    if (snapshot_read_byte_array(f, (uint8_t *)magic, SNAPSHOT_MAGIC_LEN) < 0
        || memcmp(magic, snapshot_magic_string, SNAPSHOT_MAGIC_LEN) != 0) {
        snapshot_error = SNAPSHOT_MAGIC_STRING_MISMATCH_ERROR;
        goto fail;
    }

    /* Version number.  */
    if (snapshot_read_byte(f, major_version_return) < 0
        || snapshot_read_byte(f, minor_version_return) < 0) {
        snapshot_error = SNAPSHOT_CANNOT_READ_VERSION_ERROR;
        goto fail;
    }

    /* Machine.  */
    if (snapshot_read_byte_array(f, (uint8_t *)read_name, SNAPSHOT_MACHINE_NAME_LEN) < 0) {
        snapshot_error = SNAPSHOT_CANNOT_READ_MACHINE_NAME_ERROR;
        goto fail;
    }

    /* Check machine name.  */
    machine_name_len = (int)strlen(snapshot_machine_name);
    if (memcmp(read_name, snapshot_machine_name, machine_name_len) != 0
        || (machine_name_len != SNAPSHOT_MODULE_NAME_LEN
            && read_name[machine_name_len] != 0)) {
        snapshot_error = SNAPSHOT_MACHINE_MISMATCH_ERROR;
        goto fail;
    }

    /* VICE version and revision */
    memset(snapshot_viceversion, 0, 4);
    snapshot_vicerevision = 0;
    offs = snapshot_ftell(f);

    if (snapshot_read_byte_array(f, (uint8_t *)magic, SNAPSHOT_VERSION_MAGIC_LEN) < 0
        || memcmp(magic, snapshot_version_magic_string, SNAPSHOT_VERSION_MAGIC_LEN) != 0) {
        /* old snapshots do not contain VICE version */
        snapshot_fseek(f, offs, SEEK_SET);
        log_warning(LOG_DEFAULT, "attempting to load pre 2.4.30 snapshot");
    } else {
        /* actually read the version */
        if (snapshot_read_byte(f, &snapshot_viceversion[0]) < 0
            || snapshot_read_byte(f, &snapshot_viceversion[1]) < 0
            || snapshot_read_byte(f, &snapshot_viceversion[2]) < 0
            || snapshot_read_byte(f, &snapshot_viceversion[3]) < 0
            || snapshot_read_dword(f, &snapshot_vicerevision) < 0) {
            snapshot_error = SNAPSHOT_CANNOT_READ_VERSION_ERROR;
            goto fail;
        }
    }

    s = lib_malloc(sizeof(snapshot_t));
    s->file = f;
    s->first_module_offset = snapshot_ftell(f);
    s->write_mode = 0;

    vsync_suspend_speed_eval();
    return s;

fail:
    return NULL;
}

snapshot_t *snapshot_open(const char *filename, uint8_t *major_version_return, uint8_t *minor_version_return, const char *snapshot_machine_name)
{
    snapshot_stream_t *file;
    snapshot_t *snapshot;

    file = snapshot_file_read_fopen(filename);
    snapshot = snapshot_open_from_stream(file, major_version_return, minor_version_return, snapshot_machine_name);
    if (!snapshot) {
        snapshot_fclose(file);
    }
    return snapshot;
}

int snapshot_close(snapshot_t *s)
{
    int retval;

    if (!s->write_mode) {
        if (snapshot_fclose(s->file) == EOF) {
            snapshot_error = SNAPSHOT_READ_CLOSE_EOF_ERROR;
            retval = -1;
        } else {
            retval = 0;
        }
    } else {
        if (snapshot_fclose(s->file) == EOF) {
            snapshot_error = SNAPSHOT_WRITE_CLOSE_EOF_ERROR;
            retval = -1;
        } else {
            retval = 0;
        }
    }

    lib_free(s);
    return retval;
}

int snapshot_free(snapshot_t *s)
{
    lib_free(s);
    return 0;
}

static void display_error_with_vice_version(char *text, char *filename)
{
    char *vmessage = lib_malloc(0x100);
    char *message = lib_malloc(0x100 + strlen(text));
    if ((snapshot_viceversion[0] == 0) && (snapshot_viceversion[1] == 0)) {
        /* generic message for the case when no version is present in the snapshot */
        strcpy(vmessage, "Snapshot was created by VICE Version 2.4.30 or older");
    } else {
        sprintf(vmessage, "Snapshot was created by VICE Version %d.%d.%d", snapshot_viceversion[0], snapshot_viceversion[1], snapshot_viceversion[2]);
        if (snapshot_vicerevision != 0) {
            sprintf(message, " (r%d)", (int)snapshot_vicerevision);
            strcat(vmessage, message);
        }
    }
    sprintf(message, "%s\n\n%s.", text, vmessage);
    ui_error(message, filename);
    lib_free(message);
    lib_free(vmessage);
}

void snapshot_display_error(void)
{
    switch (snapshot_error) {
        default:
        case SNAPSHOT_NO_ERROR:
            break;
        case SNAPSHOT_WRITE_EOF_ERROR:
            if (current_module) {
                ui_error("EOF while writing to module %s in snapshot %s", current_module, current_filename);
            } else {
                ui_error("EOF while writing to snapshot %s", current_filename);
            }
            break;
        case SNAPSHOT_WRITE_BYTE_ARRAY_ERROR:
            if (current_module) {
                ui_error("Error writing array to module %s in snapshot %s", current_module, current_filename);
            } else {
                ui_error("Error writing array to snapshot %s", current_filename);
            }
            break;
        case SNAPSHOT_READ_EOF_ERROR:
            if (current_module) {
                ui_error("EOF while reading from module %s in snapshot %s", current_module, current_filename);
            } else {
                ui_error("EOF while reading from snapshot %s", current_filename);
            }
            break;
        case SNAPSHOT_READ_BYTE_ARRAY_ERROR:
            if (current_module) {
                ui_error("Error reading array from module %s in snapshot %s", current_module, current_filename);
            } else {
                ui_error("Error reading array from snapshot %s", current_filename);
            }
            break;
        case SNAPSHOT_ILLEGAL_STRING_LENGTH_ERROR:
            if (current_module) {
                ui_error("Error writing string to module %s in snapshot %s", current_module, current_filename);
            } else {
                ui_error("Error writing string to snapshot %s", current_filename);
            }
            break;
        case SNAPSHOT_READ_OUT_OF_BOUNDS_ERROR:
            if (current_module) {
                ui_error("Out of bounds reading error in module %s in snapshot %s", current_module, current_filename);
            } else {
                ui_error("Out of bounds reading error in snapshot %s", current_filename);
            }
            break;
        case SNAPSHOT_ILLEGAL_OFFSET_ERROR:
            ui_error("Illegal offset while attempting to create module %s in snapshot %s", current_module, current_filename);
            break;
        case SNAPSHOT_FIRST_MODULE_NOT_FOUND_ERROR:
            ui_error("Cannot find first module in snapshot %s", current_filename);
            break;
        case SNAPSHOT_MODULE_HEADER_READ_ERROR:
            ui_error("Error while reading module header in snapshot %s", current_filename);
            break;
        case SNAPSHOT_MODULE_NOT_FOUND_ERROR:
            ui_error("Cannot find module %s in snapshot %s", current_module, current_filename);
            break;
        case SNAPSHOT_MODULE_CLOSE_ERROR:
            ui_error("Error closing module %s in snapshot %s", current_module, current_filename);
            break;
        case SNAPSHOT_MODULE_SKIP_ERROR:
            ui_error("Error skipping module in snapshot %s", current_filename);
            break;
        case SNAPSHOT_CANNOT_CREATE_SNAPSHOT_ERROR:
            ui_error("Cannot create snapshot %s", current_filename);
            break;
        case SNAPSHOT_CANNOT_WRITE_MAGIC_STRING_ERROR:
            ui_error("Cannot write magic string to snapshot %s", current_filename);
            break;
        case SNAPSHOT_CANNOT_WRITE_VERSION_ERROR:
            ui_error("Cannot write version to snapshot %s", current_filename);
            break;
        case SNAPSHOT_CANNOT_WRITE_MACHINE_NAME_ERROR:
            ui_error("Cannot write machine name to snapshot %s", current_filename);
            break;
        case SNAPSHOT_CANNOT_OPEN_FOR_READ_ERROR:
            ui_error("Cannot open snapshot %s for reading", current_filename);
            break;
        case SNAPSHOT_MAGIC_STRING_MISMATCH_ERROR:
            ui_error("Magic string mismatch in snapshot %s", current_filename);
            break;
        case SNAPSHOT_CANNOT_READ_VERSION_ERROR:
            ui_error("Cannot read version from snapshot %s", current_filename);
            break;
        case SNAPSHOT_CANNOT_READ_MACHINE_NAME_ERROR:
            ui_error("Cannot read machine name from snapshot %s", current_filename);
            break;
        case SNAPSHOT_MACHINE_MISMATCH_ERROR:
            ui_error("Wrong machine type in snapshot %s, snapshot type: %s, current machine: %s", current_filename, read_name, current_machine_name);
            break;
        case SNAPSHOT_READ_CLOSE_EOF_ERROR:
        case SNAPSHOT_WRITE_CLOSE_EOF_ERROR:
            ui_error("EOF while closing snapshot %s", current_filename);
            break;
        case SNAPSHOT_MODULE_HIGHER_VERSION:
            display_error_with_vice_version("Snapshot %s has a higher version than what your current emulator supports, please upgrade VICE", current_filename);
            break;
        case SNAPSHOT_MODULE_INCOMPATIBLE:
            display_error_with_vice_version("Snapshot %s is incompatible (too old)", current_filename);
            break;
    }
}

void snapshot_set_error(int error)
{
    snapshot_error = error;
}

int snapshot_version_at_least(uint8_t major_version, uint8_t minor_version, uint8_t major_version_required, uint8_t minor_version_required)
{
    if (major_version != major_version_required) {
        return 0;
    }

    if (minor_version >= minor_version_required) {
        return 1;
    }

    return 0;
}
