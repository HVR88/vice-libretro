The downstream BC fork's repo is private at the moment. Request access or wait until it's made public again. Windows binaries of the cores are available as Release 1.0 in the base HVR88/vice-libretro repo for now:
* https://github.com/HVR88/vice-libretro/releases/
# VICE Libretro - Border Cropping (BC) Edition - Pixelpiper 2020

This project fork adds options to fit/fill any monitor of any aspect ratio, size or resolution, with perfectly scaled, non-distorted emulator screens at each platform’s correct aspect ratio - the resulting output will closely match the proportions/dimensions originally seen on a CRT/TV.

Output and features will work with all displays/monitors: 16:9, 16:10, 5:4, 4:3, LCDs, CRTs, etc. at any desktop resolution - including portrait-rotated modes/displays.

## Features ##
* Quick Zoom/Border Cropping presets
  * Small, Medium and Full border cropping - both width & height
  * Fit to Narrow & Fit to Wide (screens) - width or height
  * Custom cropping - displays additional options below
  
* Separate Width & Height cropping options for C64, C128, Vic-20 and C16/Plus4 cores
  * Preset-based with 3 crop settings for Width & 4 for Height, including complete border removal
  * Supports PAL & NTSC for every platform with appropriately-selected cropping presets for both Width and Height
  * No buggy side-effects of the Display Border Off option

* Manual Height Cropping Option for C64 and C128 cores - make the output perfectly fit any game
  * Independent Top and Bottom Cropping settings from 0 to 60 pixels (NTSC cropping restricted to match PAL range)
  * With support for over-cropping beyond the border to work with widest-possible range of games/titles

* Three new 4:3 normalized Pixel Aspect Correction options
  * Tweak to make NTSC/PAL fill similar/same amount of space (crop safe)
  * Ratios based on fitting full Vice region-specific resolutions to 4:3 CRT

* Virtual Keyboard scaling
  * Adapts to cropped height as necessary
  
* Hot Key to toggle (ON/OFF) the currently set Quick Zoom / Cropping preset

* Hot Key to cycle through the Quick Zoom / Border Cropping presets

* Hot Key to cycle through the presets for Width Cropping

* Hot Key to cycle through the presets for Height Cropping - includes Manual Crop setting

* Reorganized & relabeled Core Options for quicker access and ease of use

## Screenshot of Core Options ##

![Screenshot](https://github.com/HVR88/HVR88_project_extras/blob/master/VICE%20Libretro%20BC%20Edition-small.png)


Requirements: **Retroarch settings: Video settings -> Scaling -> Aspect Ratio: Core Provided + Integer Scaling: OFF**

***
The new options have been adopted by the emulation community and a large collection of games have been profiled to provide the optimal crop values to work on any display in the *C64 Dreams Curated Collection:* https://forums.launchbox-app.com/topic/49324-c64-dreams-massive-curated-c64-collection

**What others have said**

*”now 100% of the cropping is handled in the core itself, [RetroArch] video settings are untouched. This is a godsend because now it should “just work” for everyone. Equally important is that now that I can adjust these crops by single pixels, they’re more exact than the previous pre-defined zoom levels.”*

*”I’ll be honest, I wish every core had this option. I would use it. At the very least Beetle PSX and Beetle Saturn could really use it.Thank you for doing this! It resolves probably the single biggest issue with this project, so that’s a major boon.”*


***

# Original Readme Below ##

# Vice 3.3 LIBRETRO

Port of VICE 3.3 for libretro with virtual keyboard

Supported platforms: Linux Windows Apple Android emscripten Switch Vita

Source base: https://sourceforge.net/projects/vice-emu/files/releases/vice-3.3.tar.gz

## Recent improvements

- Automatic NIB->G64 conversion
- Savestates
- x64sc / xpet
- Virtual keyboard revamped: more responsive, cleaner design, much easier to control
- GUI removed
- Autostart PRG mode to inject
- System-subdir renamed to `vice` and created automatically
- Cursor keys disabled while using RetroPad
- New settings:
  - Drive Sound Emulation (D64 only)
  - Reset Type (Autostart, Soft, Hard)
  - Customizable hotkeys for essential functions (virtual keyboard, statusbar, joyport switch, reset, Datasette controls)
  - Keyrah joystick maps
  - Zoom mode
  - Paddles & mouse
  - JiffyDOS support


## Default controls

|RetroPad button|Action|
|---|---|
|B|Fire button|
|L2|Run/Stop (Escape)|
|R2|Return (Enter)|
|Select|Toggle virtual keyboard|

|Keyboard key|Action|
|---|---|
|F11|Toggle virtual keyboard|
|F12|Toggle statusbar|
|RControl|Toggle between joyports|
|End|Reset|

### Virtual keyboard controls
|Button|Action|
|---|---|
|B / Enter|Keypress|
|A|Toggle transparency|
|Y|Toggle ShiftLock|
|Start|Press Return|

Long press for sticky keys. Stickying a third key will replace the second.


## Joyport control via filename

C64 games sometimes use joystick port 1 and sometimes joystick port 2 for player 1. There are several ways to switch ports in this core:
- Use the core option for it: `Quick Menu -> Options -> RetroPad Port`.
- Bring up the virtual keyboard with `Select` button, and press the key labeled `JOY` there.
- Rename your games, eg. `Bruce_Lee_j1.tap` or `Bruce_Lee_(j1).tap` for port 1, and similarly `Bruce_Lee_j2.tap` or `Bruce_Lee_(j2).tap` for port 2.

## M3U support and disk control
When you have a multi disk game, you can use a M3U file to specify each disk of the game and change them from the RetroArch Disk Control interface.

A M3U file is a simple text file with one disk per line (see https://en.wikipedia.org/wiki/M3U).

Example:

Ultima VI - The False Prophet (1990)(Origin Systems).m3u
```
Ultima VI - The False Prophet (1990)(Origin Systems)(Disk 1 of 3 Side A)(Game).d64
Ultima VI - The False Prophet (1990)(Origin Systems)(Disk 1 of 3 Side B)(Surface).d64
Ultima VI - The False Prophet (1990)(Origin Systems)(Disk 2 of 3 Side A)(Dungeon).d64
Ultima VI - The False Prophet (1990)(Origin Systems)(Disk 2 of 3 Side B)(Populace A).d64
Ultima VI - The False Prophet (1990)(Origin Systems)(Disk 3 of 3 Side A)(Populace A).d64
Ultima VI - The False Prophet (1990)(Origin Systems)(Disk 3 of 3 Side B)(Populace A).d64
```
Path can be absolute or relative to the location of the M3U file.

When a game ask for it, you can change the current disk in the RetroArch 'Disk Control' menu:
- Eject the current disk with 'Disk Cycle Tray Status'.
- Select the right disk index.
- Insert the new disk with 'Disk Cycle Tray Status'.

Note: ZIP support is provided by the core, which allows the use of zipped images in M3Us.


## JiffyDOS support

External ROM files required in `system/vice`:

|Filename|Size|MD5|
|---|---|---|
|**JiffyDOS_C64.bin**|8 192|be09394f0576cf81fa8bacf634daf9a2|
|**JiffyDOS_C128.bin**|16 384|cbbd1bbcb5e4fd8046b6030ab71fc021|
|**JiffyDOS_1541-II.bin**|16 384|1b1e985ea5325a1f46eb7fd9681707bf|
|**JiffyDOS_1571_repl310654.bin**|32 768|41c6cc528e9515ffd0ed9b180f8467c0|
|**JiffyDOS_1581.bin**|32 768|20b6885c6dc2d42c38754a365b043d71|


## Command file operation

VICE command line options are supported with RetroArch by by placing the desired command line in a text file with a `.cmd` file extension. The command line format is as documented in the Vice documentation (see http://vice-emu.sourceforge.net/vice_6.html).

Using this you can overcome limitations of the GUI and set advanced configurations required for running problematic files. Here are a couple of examples for the xvic core  ...

VIC-20 megacart support:
```
xvic -cartmega /path/to/rom/mega-cart-name.rom
```
VIC-20 games which require specific memory configs such as 8k expansion:
```
xvic -memory 1 /path/to/rom/some-8k-game.d64
```

VIC-20 memory expansion will also be set with filename tags or directory matching: `some game (8k).prg` or `/8k/some game.prg`.

## Build instructions

Remember to run `make clean EMUTYPE=x` when building different EMUTYPEs.

Currently working EMUTYPEs:
- `x64`
- `x64sc`
- `x128`
- `xplus4`
- `xvic`
- `xpet`
- `xcbm2`

### Linux
```
CC=gcc make -j4
```
### Windows
```
make -j4
```
### Win64 (crossbuild)
```
make platform=wincross64 -j4
```
### Android standalone toolchain Build
```
export path to your standalone toolchain like

export PATH=$PATH:/opt/standtc/bin

make platform=androidstc -j4
```
### Android ndk Build
```
cd jni
ndk-build
```
## Original readme

 ----------------------------------------------------------------------------

  VICE  3.3                                                        Dec 2018


                 _______________
                |      ||      ||
                |      ||      ||____________________________
                |      ||      ||    ||         ||          ||
                |      ||      ||    ||    |    ||    ______||
                |      ||      ||    ||    |----||          ||
                |      ||      ||    ||    |    ||    ------||
                |              ||____||_________||__________||
                 \            // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                  \          //
                   \________//   Versatile Commodore 8-bit Emulator



 ----------------------------------------------------------------------------

 This release stabilizes and introduces the GTK3 UI as the primary user 
 interface for all major ports. For "lesser" platforms we still have the SDL 
 based interface. All other user interfaces have been removed. RIP

 We are still looking for dedicated maintainers for the Windows- and macOS 
 ports, if you want to help improving these and/or want to provide binaries, 
 please get in touch.

 ----------------------------------------------------------------------------

 This is version 3.3 of VICE, the multi-platform C64, C128, VIC20,
 PET, PLUS4 and CBM-II emulator.  This version can be compiled for
 MSDOS, Win32, OS/2, BeOS, QNX 4.x, QNX 6.x, AmigaOS, Dingoo,
 Syllable, SkyOS and for most Unix systems provided with
 the X Window System version 11, R5 or later.

 The following programs are included:

   - x64, a C64 emulator;

   - x64sc, a C64 emulator focused on accuracy;

   - xscpu64, a C64 emulator with a SuperCPU cart;

   - x64dtv, a C64 Direct-to-TV (DTV) emulator;

   - x128, a C128 emulator;

   - xvic, a VIC20 emulator;

   - xpet, a PET emulator;

   - xplus4, a PLUS4 emulator;

   - xcbm2, a CBM-6x0/7x0 emulator;

   - xcbm5x0, a CBM-5x0 emulator;

   - vsid, a SID player;

   - c1541, a stand-alone disk image maintenance utility;

   - petcat, a CBM BASIC de-tokenizer;

   - cartconv, a C64/C128 cartridge conversion program.

 Moreover, the following documents are provided in both source and
 binary distributions:

   - README, this file;

   - COPYING, the GNU General Public License under which VICE is
     distributed -- *please read it before using the program*;

   - INSTALL, installation instructions;

   - NEWS, list of user-visible changes between this and older versions of VICE;

   - FEEDBACK, something you should read before reporting bugs or sending mail 
     to any of the authors;

 The following documents are provided in the source distribution:

   - in the doc directory:

     - vice.texi, the complete documentation; documentation in various different
       formats may be generated from it (vice.txt, vice.pdf, vice.chm, vice.hlp, 
       vice.guide, vice.inf, vice.info)
     - iec-bus.txt, overview of the VICE IEC bus emulation;
     - CIA-README.txt, overview of the VICE CIA emulation;
     - coding-guidelines.txt, description of VICE coding style
     - Documentation-Howto.txt, information on how to edit the documentation
     - Doxygen-Howto.txt, how to generate doxygen documentation
     - Release-Howto.txt, some hints and reminders on how to create a release
       tarball (for maintainers)
     - ffmpeg-support.txt, this document details the results of test compiles 
       with regards to the included ffmpeg library.
     - SDL-support.txt, this document details the results of test compiles with 
       regards to the SDL 1 and SDL 2 support.
     - hardware-sids.txt, this document details the hardware SIDs and how they 
       are supported on the various platforms.

   - in the doc/readmes directory:

     - Readme-GTK3.txt, GTK3 specific documentation;
     - Readme-SDL.txt, SDL specific documentation;
     - Readme-SDL2.txt, SDL2 specific documentation;

   - in the doc/building directory:

     - Windows-MinGW-GTK3-Howto.txt, how to compile the Windows port using MingGW/MSYS2
     - Linux-GTK3-Howto.txt, how to compile the GTK3 port on Linux
     - GTK3-macOS.txt, how to compile the Mac OSX port with GTK3 UI
     - SDL-Howto.txt, how to compile the SDL port on various platforms
     - Android-Howto, how to compile the Android SDL port
     - FreeBSD-GTK3-Howto.txt, hints on compiling the GTK3 port on FreeBSD
     - NetBSD-howto.txt, hints on compiling for NetBSD

 For the latest news, have a look at the VICE home page:

     http://vice-emu.sourceforge.net/

 Also some information that was previously included in the distribution has been
 moved into our wiki at sourceforge:

   - The TODO list,
     http://vice-emu.sourceforge.net/wiki/index.php/Todo#New_Features

   - The list of known BUGS,
     http://vice-emu.sourceforge.net/wiki/index.php/Todo#Known_Issues

 New versions are made available quite often, so please stay tuned.


 Have fun!


 Copyright notice
 ----------------

 VICE, the Versatile Commodore Emulator

    Core Team Members:
    2005-2018 Marco van den Heuvel
    2007-2018 Fabrizio Gennari
    2009-2018 Groepaz
    2010-2018 Olaf Seibert
    2011-2018 Marcus Sutton
    2011-2018 Kajtar Zsolt
    2016-2018 AreaScout
    2016-2018 Bas Wassink
    2017-2018 Michael C. Martin

    Inactive/Ex Team Members:
    1999-2017 Martin Pottendorfer
    2011-2016 Stefan Haubenthal
    2015-2016 BSzili
    1999-2016 Andreas Matthies
    2007-2015 Daniel Kahlin
    2009-2015 Errol Smith
    2012-2014 Benjamin 'BeRo' Rosseaux
    2011-2014 Ulrich Schulz
    2011-2014 Thomas Giesel
    2009-2014 Ingo Korb
    2008-2014 Antti S. Lankila
    2006-2014 Christian Vogelgsang
    1998-2014 Dag Lem
    2000-2011 Spiro Trikaliotis
    2007-2011 Hannu Nuotio
    1998-2010 Andreas Boose
    1998-2010 Tibor Biczo
    2007-2010 M. Kiesel
    1999-2007 Andreas Dehmel
    2003-2005 David Hansel
    2000-2004 Markus Brenner
    1999-2004 Thomas Bretz
    1997-2001 Daniel Sladic
    1996-2001 André Fachat
    1996-1999 Ettore Perazzoli
    1993-1994, 1997-1999 Teemu Rantanen
    1993-1996 Jouko Valta
    1993-1994 Jarkko Sonninen

    Translation Team Members:
    2009-2017 Mikkel Holm Olsen
    2000-2017 Martin Pottendorfer
    2011-2017 Manuel Antonio Rodriguez Bas
    2004-2017 Paul Dubé
    2006-2017 Czirkos Zoltan
    2006-2017 Karai Csaba
    2001-2017 Andrea Musuruane
    2011-2016 Jesse Lee
    2005-2017 Marco van den Heuvel
    2011-2017 Jarek Sobolewski
    2010-2017 Michael Litvinov
    2000-2017 Peter Krefting
    2008-2017 Emir Akaydin

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307  USA

 The ROM files in the `C128', `C64', `CBM-II', `DRIVES', `PET', `PLUS4'
 `PRINTER' and `VIC20' directories are Copyright C by Commodore
 Business Machines.

 The ROM files in the `C64DTV` directory are Copyright C by Commodore
 Business Machines, as well as Mammoth Toys, a division of nsi ltd.,
 Digital Concepts DC studios inc., Ironstone Partners ltd., and
 Toy:Lobster company ltd.


### NIBTOOLS

(C) 2005 Pete Rittwage and the C64 Preservation Project team

It is based on MNIB which is copyrighted
(C) 2000 Markus Brenner

