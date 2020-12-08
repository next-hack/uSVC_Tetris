/*
*  uChip Tetris, another Tetris clone, this time for uChip Simple VGA Console (uSVC)
*  Copyright 2019-2020 Nicola Wrachien (next-hack.com)
*
*  This game demonstrates the capabilities of the Bitmapped Mode, allowing to show up to
*  256 colors (limitations apply).
*
*  This file is part of uChip Tetris.
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program  is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  tl;dr
*  Do whatever you want, this program is free! Though we won't
*  reject donations https://next-hack.com/index.php/donate/ :)
*
*  usvc_config.h. this file configures the usvc kernel.
*/

#ifndef USVC_CONFIG_H_
#define USVC_CONFIG_H_
/*
	FIXED SECTION
	TILEMODE allows for a fixed section (no-scroll) at the top or the bottom of the screen.
	To activate it: 
	define USE_SECTION as TOP_FIXED_SECTION or BOTTOM_FIXED_SECTION
	eg, for a bottom fixed section:
	#define USE_SECTION TOP_FIXED_SECTION
	Also set the section limit, i.e. the row at which the section starts (for bottom) or ends (for top). The number is in terms of tiles and it is not inclusive.
*/
//#define GFX_MODE TILE_MODE1
#define GFX_MODE BITMAPPED_MODE
#define PER_HORIZONTAL_BLOCK_PALETTE_REMAP 1
#define MAX_NUMBER_OF_PALETTE_ENTRIES (1024)
#define AUDIO_ENABLED 1
#define USE_MIXER 1
#define ENABLE_TILE_PRIORITY 1
#define ENABLE_SPRITE_PRIORITY 0
// module inclusion. Set to 1 to force inclusion of the module, so that your game will use the one you are providing. Set to 0, and the game will use 
// the bootloader's built in functions (allowing to save more than 10kB)
#define FORCE_INCLUDE_FATFS 0
#ifndef USE_BOOTLOADER
	#define FORCE_INCLUDE_USB_GAMEPAD_MODULE 1 // 0: use bootloader libraries and save space
	#define FORCE_INCLUDE_USB_MODULE 1			// 0: use bootloader libraries and save space
	#define FORCE_INCLUDE_USB_KEYBOARD_MODULE 1	// 0: use bootloader libraries and save space
#endif
#define INCLUDE_DEFAULT_WAVES 1
#define AUDIO_USES_LPF 0
#define MUSIC_ENGINE MIDI
#define SPRITES_ENABLED 0
// USB CONFIGURATION: MAX 2 Pipes and 1 device. Modify according to your needs. Be aware that HUB does not support low speed devices!
#define USE_USB_HUB 0
#define USB_NUMDEVICES 1
#define NUMBER_OF_USB_PIPES 3
#define MAX_USB_INTERFACES 4
// PUT HERE THE INCLUDES TO ALL YOUR DATA (for instance audio patches, sounds and sprites)
#if AUDIO_ENABLED == 1
#include "mainTheme.h"
#include "soundWaveList.h"
#include "soundData.h"
#include "patches.h"
#endif
#if SPRITES_ENABLED == 1
#include "VGAspriteData.h"
#if ENABLE_TRANSPARENT_SPRITES == 1
	#include "colorTables.h"
#endif 
#endif



#endif /* USVC_CONFIG_H_ */