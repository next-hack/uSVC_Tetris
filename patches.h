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
*  patches.c/h. Sound effects for uChip Tetris.
*/
#ifndef PATCHES_H
#define PATCHES_H
#include "usvc_kernel/audio.h"
#define ROTATE_PATCH 3
#define MENU_MOVE_PATCH 4
#define MENU_ERROR_PATCH 5
#define LINE_CLEAR_PATCH 6
#define LEVEL_CLEAR_PATCH 7
#define GAME_OVER_PATCH 8
#define FREE_FALL_PATCH 9
extern const  patch_t patches[];
int getNumberOfPatches();
#endif
