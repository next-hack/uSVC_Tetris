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
*  tetrisBlocks.c/h. the game block graphics.
*/
#ifndef TETRISBLOCKS_H_
#define TETRISBLOCKS_H_
#include <stdint.h>
#define TETRISBLOCKS_PALETTE_ENTRIES 216
#define TETRISBLOCKS_WIDTH 8
#define TETRISBLOCKS_HEIGHT 192
#define TETRISBLOCKS_PIC_SIZE (TETRISBLOCKS_HEIGHT * TETRISBLOCKS_WIDTH / 4)
#define TETRISBLOCKS_PALETTE_INDEX_TABLE_SIZE (TETRISBLOCKS_HEIGHT * TETRISBLOCKS_WIDTH / 8)
extern const uint8_t tetrisBlocks_data[TETRISBLOCKS_PIC_SIZE];
extern const uint32_t tetrisBlocks_palettes[TETRISBLOCKS_PALETTE_ENTRIES];
extern const uint8_t tetrisBlocks_paletteIndexTable[TETRISBLOCKS_PALETTE_INDEX_TABLE_SIZE];
#endif /* TETRISBLOCKS_H_ */