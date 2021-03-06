/*
 * uistatusbar.c - SDL statusbar.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *
 * Based on code by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "uistatusbar.h"
#include "videoarch.h"

#include "libretro.h"
#include "libretro-core.h"
#include "graph.h"
#include "file/file_path.h"
#include "retro_miscellaneous.h"

#include "joystick.h"
#include "archdep.h"

extern unsigned int opt_joyport_type;
extern unsigned int mouse_value[2 + 1];

/* ----------------------------------------------------------------- */
/* static functions/variables */

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define MAX_STATUSBAR_LEN           48
#define STATUSBAR_JOY_POS           0
#define STATUSBAR_DRIVE_POS         41
#define STATUSBAR_DRIVE8_TRACK_POS  43
#define STATUSBAR_DRIVE9_TRACK_POS  43
#define STATUSBAR_DRIVE10_TRACK_POS 43
#define STATUSBAR_DRIVE11_TRACK_POS 43
#define STATUSBAR_TAPE_POS          39
#define STATUSBAR_PAUSE_POS         45
#define STATUSBAR_SPEED_POS         45

static char statusbar_text[MAX_STATUSBAR_LEN] = {0};

static char* joystick_value_human(char val, int vice_device)
{
    static char str[6] = {0};
    sprintf(str, "%3s", "   ");

    if (val & 0x01) // UP
        str[1] = 30;

    else if (val & 0x02) // DOWN
        str[1] = 28;

    if (val & 0x04) // LEFT
        str[0] = 27;

    else if (val & 0x08) // RIGHT
        str[2] = 29;

    str[1] = (val & 0x10) ? (str[1] | 0x80) : str[1];

    if (vice_device > 0)
    {
        str[1] = (val & 0x10) ? ('L' | 0x80) : str[1];
        str[1] = (val & 0x20) ? ('R' | 0x80) : str[1];
        str[1] = (val & 0x40) ? ('M' | 0x80) : str[1];
    }

    return str;
}

static void display_joyport(void)
{
    int len;
    char tmpstr[25];

#if !defined(__PET__) && !defined(__CBM2__) && !defined(__VIC20__)
    char joy1[2];
    char joy2[2];
    sprintf(joy1, "%s", "1");
    sprintf(joy2, "%s", "2");
    if(cur_port == 1)
        joy1[0] = (joy1[0] | 0x80);
    else if(cur_port == 2)
        joy2[0] = (joy2[0] | 0x80);

    // Mouse
    if (opt_joyport_type > 2 && cur_port == 1)
        sprintf(tmpstr, "M%s%3s ", joy1, joystick_value_human(mouse_value[1], 1));
    // Paddles
    else if (opt_joyport_type == 2 && cur_port == 1)
        sprintf(tmpstr, "P%s%3s ", joy1, joystick_value_human(mouse_value[1], 1));
    // Joystick
    else
        sprintf(tmpstr, "J%s%3s ", joy1, joystick_value_human(joystick_value[1], 0));

    // Mouse
    if (opt_joyport_type > 2 && cur_port == 2)
        sprintf(tmpstr + strlen(tmpstr), "M%s%3s ", joy2, joystick_value_human(mouse_value[2], 1));
    // Paddles
    else if (opt_joyport_type == 2 && cur_port == 2)
        sprintf(tmpstr + strlen(tmpstr), "P%s%3s ", joy2, joystick_value_human(mouse_value[2], 1));
    // Joystick
    else
        sprintf(tmpstr + strlen(tmpstr), "J%s%3s ", joy2, joystick_value_human(joystick_value[2], 0));

    if (RETROUSERPORTJOY != -1)
    {
        sprintf(tmpstr + strlen(tmpstr), "J%d%3s ", 3, joystick_value_human(joystick_value[3], 0));
        sprintf(tmpstr + strlen(tmpstr), "J%d%3s ", 4, joystick_value_human(joystick_value[4], 0));
    }
    else
    {
        sprintf(tmpstr + strlen(tmpstr), "%5s", "");
        sprintf(tmpstr + strlen(tmpstr), "%5s", "");
    }
#else
    char joy1[2];
    sprintf(joy1, "%s", "1");

    sprintf(tmpstr, "J%s%3s ", joy1, joystick_value_human(joystick_value[1], 0));
#endif

    if (opt_statusbar & STATUSBAR_BASIC)
        snprintf(tmpstr, sizeof(tmpstr), "%24s", "");

    len = sprintf(&(statusbar_text[STATUSBAR_JOY_POS]), "%-38s", tmpstr);
    //statusbar_text[STATUSBAR_JOY_POS + len] = ' ';

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

static int per = 0;
static int fps = 0;
static int warp = 0;
static int paused = 0;

static void display_speed(void)
{
    int len;
    //char sep = paused ? ('P' | 0x80) : warp ? ('W' | 0x80) : ' ';
    char fps_str[3];
    sprintf(fps_str, "%2d", fps);
    //sprintf(fps_str, "%2s", "");
    //if (warp)
    {
        fps_str[0] = (fps_str[0] | 0x80);
        fps_str[1] = (fps_str[1] | 0x80);
    }

    len = sprintf(&(statusbar_text[STATUSBAR_SPEED_POS]), "%2s", fps_str);
    //statusbar_text[STATUSBAR_SPEED_POS + len] = ' '; /* No end separator for last element */

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

int imagename_timer = 0;
static int drive_enabled = 0;
static int drive_empty = 0;

void display_current_image(const char *image, bool inserted)
{
    static char imagename[PATH_MAX_LENGTH] = {0};
    static char imagename_prev[PATH_MAX_LENGTH] = {0};
    static char imagelabel[PATH_MAX_LENGTH] = {0};
    imagename_timer = 150;

    if (strcmp(image, ""))
    {
        drive_empty = (inserted) ? 0 : 1;
        snprintf(imagelabel, sizeof(imagelabel), "%s", (char*)image);
        snprintf(imagelabel, sizeof(imagelabel), "%s", path_basename(imagelabel));
        snprintf(imagelabel, sizeof(imagelabel), "%s", path_remove_extension(imagelabel));
        snprintf(imagename, sizeof(imagename), "%2s%.36s", "  ", imagelabel);
        snprintf(imagename_prev, sizeof(imagename_prev), "%.38s", imagename);
    }
    else
    {
        drive_empty = 1;
        snprintf(imagename, sizeof(imagename), "%.38s", imagename_prev);
    }

    snprintf(&statusbar_text[STATUSBAR_JOY_POS], sizeof(statusbar_text), "%-38s", imagename);

    if (inserted)
        statusbar_text[0] = 8 | 0x80;
    else if (!strcmp(image, ""))
        statusbar_text[0] = 9 | 0x80;

    if (drive_empty)
    {
        statusbar_text[STATUSBAR_DRIVE8_TRACK_POS] = ' ';
        statusbar_text[STATUSBAR_DRIVE8_TRACK_POS + 1] = ' ';
    }
    else if (drive_enabled)
    {
        statusbar_text[STATUSBAR_DRIVE8_TRACK_POS] = '0';
        statusbar_text[STATUSBAR_DRIVE8_TRACK_POS + 1] = '0';
    }
}

/* ----------------------------------------------------------------- */
/* ui.h */

void ui_display_speed(float percent, float framerate, int warp_flag)
{
    per = (int)(percent + .5);
    if (per > 999) {
        per = 999;
    }

    fps = (int)(framerate + .5);
    if (fps > 99) {
        fps = 99;
    }

    warp = warp_flag;

    display_speed();
}

void ui_display_paused(int flag)
{
    paused = flag;

    display_speed();
}

/* ----------------------------------------------------------------- */
/* uiapi.h */

/* Display a mesage without interrupting emulation */
void ui_display_statustext(const char *text, int fade_out)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: \"%s\", %i\n", __func__, text, fade_out);
#endif
}

/* Drive related UI */
void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    int drive_number;
    int drive_state = (int)state;
    drive_enabled = drive_state;

    for (drive_number = 0; drive_number < 4; ++drive_number) {
        if (drive_state & 1) {
            ui_display_drive_led(drive_number, 0, 0);
        } else {
            statusbar_text[STATUSBAR_DRIVE_POS + drive_number] = ' ';
        }
        drive_state >>= 1;
    }

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

void ui_display_drive_track(unsigned int drive_number, unsigned int drive_base, unsigned int half_track_number)
{
    if (drive_empty)
        return;

    unsigned int track_number = half_track_number / 2;

#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif

    switch (drive_number) {
        case 1:
            statusbar_text[STATUSBAR_DRIVE9_TRACK_POS] = (track_number / 10) + '0';
            statusbar_text[STATUSBAR_DRIVE9_TRACK_POS + 1] = (track_number % 10) + '0';
            break;
        case 2:
            statusbar_text[STATUSBAR_DRIVE10_TRACK_POS] = (track_number / 10) + '0';
            statusbar_text[STATUSBAR_DRIVE10_TRACK_POS + 1] = (track_number % 10) + '0';
            break;
        case 3:
            statusbar_text[STATUSBAR_DRIVE11_TRACK_POS] = (track_number / 10) + '0';
            statusbar_text[STATUSBAR_DRIVE11_TRACK_POS + 1] = (track_number % 10) + '0';
            break;
        default:
        case 0:
            statusbar_text[STATUSBAR_DRIVE8_TRACK_POS] = (track_number / 10) + '0';
            statusbar_text[STATUSBAR_DRIVE8_TRACK_POS + 1] = (track_number % 10) + '0';
            break;
    }

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

static int drive_pwm = 0;

/* The pwm value will vary between 0 and 1000.  */
void ui_display_drive_led(int drive_number, unsigned int pwm1, unsigned int led_pwm2)
{
    drive_pwm = pwm1;
    return;

    char c;
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: drive %i, pwm1 = %i, led_pwm2 = %u\n", __func__, drive_number, pwm1, led_pwm2);
#endif

    c = "8901"[drive_number] | ((pwm1 > 500) ? 0x80 : 0);
    statusbar_text[STATUSBAR_DRIVE_POS + (drive_number * 5)] = c;
    statusbar_text[STATUSBAR_DRIVE_POS + (drive_number * 5) + 1] = 'T';

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

void ui_display_drive_current_image(unsigned int drive_number, const char *image)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif
}

/* Tape related UI */

static int tape_counter = 0;
static int tape_enabled = 0;
static int tape_motor = 0;
static int tape_control = 0;

static void display_tape(void)
{
    if (drive_enabled)
        return;

    int len;

    if (tape_enabled) {
        len = sprintf(&(statusbar_text[STATUSBAR_TAPE_POS]), "%c%03d%c", (tape_motor) ? '*' : ' ', tape_counter, " >f<R"[tape_control]);
    } else {
        len = sprintf(&(statusbar_text[STATUSBAR_TAPE_POS]), "     ");
    }
    //statusbar_text[STATUSBAR_TAPE_POS + len] = ' ';

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_state |= UISTATUSBAR_REPAINT;
    }
}

void ui_set_tape_status(int tape_status)
{
    tape_enabled = tape_status;

    display_tape();
}

void ui_display_tape_motor_status(int motor)
{
    tape_motor = motor;

    display_tape();
}

void ui_display_tape_control_status(int control)
{
    tape_control = control;

    display_tape();
}

void ui_display_tape_counter(int counter)
{
    if (tape_counter != counter) {
        display_tape();
    }

    tape_counter = counter;
}

void ui_display_tape_current_image(const char *image)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %s\n", __func__, image);
#endif
}

/* Recording UI */
void ui_display_playback(int playback_status, char *version)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i, \"%s\"\n", __func__, playback_status, version);
#endif
}

void ui_display_recording(int recording_status)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i\n", __func__, recording_status);
#endif
}

void ui_display_event_time(unsigned int current, unsigned int total)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i, %i\n", __func__, current, total);
#endif
}

/* Joystick UI */
void ui_display_joyport(BYTE *joyport)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %02x %02x %02x %02x %02x\n", __func__, joyport[0], joyport[1], joyport[2], joyport[3], joyport[4]);
#endif
}

/* Volume UI */
void ui_display_volume(int vol)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s: %i\n", __func__, vol);
#endif
}

/* ----------------------------------------------------------------- */
/* resources */

static int statusbar_enabled;

static int set_statusbar(int val, void *param)
{
    statusbar_enabled = val ? 1 : 0;

    if (statusbar_enabled) {
        uistatusbar_open();
    } else {
        uistatusbar_close();
    }

    return 0;
}

static const resource_int_t resources_int[] = {
    { "SDLStatusbar", 0, RES_EVENT_NO, NULL,
      &statusbar_enabled, set_statusbar, NULL },
    RESOURCE_INT_LIST_END
};

int uistatusbar_init_resources(void)
{
#ifdef SDL_DEBUG
    fprintf(stderr, "%s\n", __func__);
#endif
    return resources_register_int(resources_int);
}


/* ----------------------------------------------------------------- */
/* uistatusbar.h */

int uistatusbar_state = 0;

void uistatusbar_open(void)
{
    uistatusbar_state = UISTATUSBAR_ACTIVE | UISTATUSBAR_REPAINT;
}

void uistatusbar_close(void)
{
    uistatusbar_state = UISTATUSBAR_REPAINT;
}

void uistatusbar_draw(void)
{
    int i;
    BYTE c;

    unsigned short int color_f_16, color_b_16;
    unsigned short int color_black_16, color_white_16, color_red_16, color_green_16, color_greend_16, color_brown_16;
    color_white_16  = RGB565(255, 255, 255);
    color_black_16  = 0;//RGB565(0, 4, 0);
    color_red_16    = RGB565(187, 0 , 0);
    color_green_16  = RGB565(0, 187, 0);
    color_greend_16 = RGB565(0, 68 , 0);
    color_brown_16  = RGB565(89, 79, 78);
    color_f_16      = color_white_16;
    color_b_16      = color_black_16;

    unsigned int color_f_32, color_b_32;
    unsigned int color_black_32, color_white_32, color_red_32, color_green_32, color_greend_32, color_brown_32;
    color_white_32  = ARGB888(255, 255, 255, 255);
    color_black_32  = 0;//0x00010101;
    color_red_32    = ARGB888(255, 187, 0, 0);//0xffbb0000;
    color_green_32  = ARGB888(255, 0, 187, 0);//0xff00bb00;
    color_greend_32 = ARGB888(255, 0, 68, 0);//0xff004400;
    color_brown_32  = ARGB888(255, 89, 79, 78);
    color_f_32      = color_white_32;
    color_b_32      = color_black_32;

    unsigned int char_width = 7;
    unsigned int char_offset = 1;

    char tmpstr[512];
    int x = 0, y = 0;

    // Statusbar position
    x = 1;
    if (opt_statusbar & STATUSBAR_TOP)
        y = zoomed_YS_offset + 1;
    else
        y = zoomed_height + zoomed_YS_offset - char_width - 1;

    // Statusbar background
    int bkg_x = retroXS_offset + x - 1;
    int bkg_y = y - 1;
    int max_width = zoomed_width;
    int bkg_width = max_width;
    int bkg_height = char_width + 2;

    // Right alignment offset
    int x_align_offset = 4;
    if (retroW != zoomed_width && retroXS_offset != 0)
        x_align_offset += 1;

    // Basic mode statusbar background
    if (opt_statusbar & STATUSBAR_BASIC && imagename_timer == 0)
    {
        if (drive_enabled)
            bkg_width = (char_width * 5) - x_align_offset;
        else if (tape_enabled)
            bkg_width = (char_width * 8) - x_align_offset;
        else
            bkg_width = (char_width * 3) - x_align_offset - 1;

        bkg_x = retroXS_offset + x + max_width - bkg_width - 1;
    }

    if (pix_bytes == 2)
        DrawFBoxBmp(retro_bmp, bkg_x, bkg_y, bkg_width, bkg_height, 0, 255);
    else
        DrawFBoxBmp32((uint32_t *)retro_bmp, bkg_x, bkg_y, bkg_width, bkg_height, 0, 255);

    if (imagename_timer == 0)
        display_joyport();

    for (i = 0; i < MAX_STATUSBAR_LEN; ++i)
    {
        c = statusbar_text[i];
        if (c == 0)
            continue;
        
        // Default background
        color_b_16 = color_black_16;
        color_b_32 = color_black_32;

        // Drive/tape LED color
        if (i >= STATUSBAR_TAPE_POS && i < STATUSBAR_SPEED_POS - 1)
        {
            if (drive_enabled)
            {
                color_f_16 = color_green_16;
                color_f_32 = color_green_32;
            }
            else if (tape_enabled)
            {
                color_b_16 = color_brown_16;
                color_b_32 = color_brown_32;
                color_f_16 = color_black_16;
                color_f_32 = color_black_32;
            }
        }

        // Drive loading
        if ((i == STATUSBAR_DRIVE8_TRACK_POS || i == STATUSBAR_DRIVE8_TRACK_POS + 1) && drive_enabled)
        {
            if (drive_pwm > 300)
            {
                c = c | 0x80;
                if (opt_statusbar & STATUSBAR_MINIMAL)
                {
                    color_b_16 = color_green_16;
                    color_b_32 = color_green_32;
                }
            }
            else
            {
                color_b_16 = color_greend_16;
                color_b_32 = color_greend_32;
                color_f_16 = color_black_16;
                color_f_32 = color_black_32;
            }

            if (opt_statusbar & STATUSBAR_MINIMAL)
                c = ' ';
        }
        // Power LED color
        else if (i == STATUSBAR_SPEED_POS || i == STATUSBAR_SPEED_POS + 1)
        {
            color_f_16 = color_red_16;
            color_f_32 = color_red_32;

            if (opt_statusbar & STATUSBAR_MINIMAL)
            {
                c = ' ';
                color_b_16 = color_red_16;
                color_b_32 = color_red_32;
            }
        }

        // Right alignment for tape/drive/power
        int x_align = retroXS_offset;
        if (i >= STATUSBAR_TAPE_POS)
            x_align = retroXS_offset + x_align_offset + zoomed_width - (MAX_STATUSBAR_LEN * char_width);

        if (drive_enabled)
        {
            if (i == STATUSBAR_DRIVE8_TRACK_POS || i == STATUSBAR_DRIVE8_TRACK_POS + 1)
                x_align--;
        }
        else if (tape_enabled)
        {
            if (i >= STATUSBAR_TAPE_POS && i < STATUSBAR_SPEED_POS)
                x_align = x_align - 1 + char_width;
        }

        int x_char = x + char_offset + x_align + (i * char_width);

        // Output
        sprintf(tmpstr, "%c", c);
        if (pix_bytes == 2)
            Draw_text(retro_bmp, x_char, y, color_f_16, color_b_16, 255, 1, 1, 10, tmpstr);
        else
            Draw_text32((uint32_t *)retro_bmp, x_char, y, color_f_32, color_b_32, 255, 1, 1, 10, tmpstr);
    }
}
