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
*  GameImages.c/h. Some pictures to be shown during the game.
*/


#ifndef GAMEIMAGES_H_
#define GAMEIMAGES_H_
#include <stdint.h>
#define PIC0_PALETTE_ENTRIES 780
#define PIC0_WIDTH 80
#define PIC0_HEIGHT 80
#define PIC0_PIC_SIZE (PIC0_HEIGHT * PIC0_WIDTH / 4)
#define PIC0_PALETTE_INDEX_TABLE_SIZE (PIC0_HEIGHT * PIC0_WIDTH / 8)
extern const uint8_t pic0_data[PIC0_PIC_SIZE];
extern const uint32_t pic0_palettes[PIC0_PALETTE_ENTRIES];
extern const uint8_t pic0_paletteIndexTable[PIC0_PALETTE_INDEX_TABLE_SIZE];
#define PIC1_PALETTE_ENTRIES 764
#define PIC1_WIDTH 80
#define PIC1_HEIGHT 80
#define PIC1_PIC_SIZE (PIC1_HEIGHT * PIC1_WIDTH / 4)
#define PIC1_PALETTE_INDEX_TABLE_SIZE (PIC1_HEIGHT * PIC1_WIDTH / 8)
extern const uint8_t pic1_data[PIC1_PIC_SIZE];
extern const uint32_t pic1_palettes[PIC1_PALETTE_ENTRIES];
extern const uint8_t pic1_paletteIndexTable[PIC1_PALETTE_INDEX_TABLE_SIZE];
#define PIC2_PALETTE_ENTRIES 788
#define PIC2_WIDTH 80
#define PIC2_HEIGHT 80
#define PIC2_PIC_SIZE (PIC2_HEIGHT * PIC2_WIDTH / 4)
#define PIC2_PALETTE_INDEX_TABLE_SIZE (PIC2_HEIGHT * PIC2_WIDTH / 8)
extern const uint8_t pic2_data[PIC2_PIC_SIZE];
extern const uint32_t pic2_palettes[PIC2_PALETTE_ENTRIES];
extern const uint8_t pic2_paletteIndexTable[PIC2_PALETTE_INDEX_TABLE_SIZE];
#define PIC3_PALETTE_ENTRIES 700
#define PIC3_WIDTH 80
#define PIC3_HEIGHT 80
#define PIC3_PIC_SIZE (PIC3_HEIGHT * PIC3_WIDTH / 4)
#define PIC3_PALETTE_INDEX_TABLE_SIZE (PIC3_HEIGHT * PIC3_WIDTH / 8)
extern const uint8_t pic3_data[PIC3_PIC_SIZE];
extern const uint32_t pic3_palettes[PIC3_PALETTE_ENTRIES];
extern const uint8_t pic3_paletteIndexTable[PIC3_PALETTE_INDEX_TABLE_SIZE];
#define PIC4_PALETTE_ENTRIES 760
#define PIC4_WIDTH 80
#define PIC4_HEIGHT 80
#define PIC4_PIC_SIZE (PIC4_HEIGHT * PIC4_WIDTH / 4)
#define PIC4_PALETTE_INDEX_TABLE_SIZE (PIC4_HEIGHT * PIC4_WIDTH / 8)
extern const uint8_t pic4_data[PIC4_PIC_SIZE];
extern const uint32_t pic4_palettes[PIC4_PALETTE_ENTRIES];
extern const uint8_t pic4_paletteIndexTable[PIC4_PALETTE_INDEX_TABLE_SIZE];
extern const void *picData[];
extern const void *picPalettes[];
extern const void *picIndexTables[];
extern const uint16_t paletteEntries[];
uint8_t getNumberOfImages();
#endif /* GAMEIMAGES_H_ */