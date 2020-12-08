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
#include "main.h" 
const char synthVibraphonePatch[] =
{
	0, PC_TREMOLO_RATE, 40,
	0, PC_TREMOLO_LEVEL, 50,
	0,PC_ENV_SPEED,-5,
	0,PATCH_END
};
const char bassPatch[] =
{
	0,PC_ENV_VOL, 255,
	0,PC_ENV_SPEED,0,
	0,PATCH_END
};
// rotate
const char rotatePatch[] =
{
	0, PC_WAVE, 0,
	0, PC_PITCH, 80,
	0, PC_ENV_SPEED, -10,
	1, PC_NOTE_UP, 1,
	1, PC_NOTE_UP, 2,
	1, PC_NOTE_UP, 1,
	5, PC_NOTE_DOWN, 3,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_CUT, 0,
	0, PATCH_END,
};
// move
const char menuMove[] =
{
	0, PC_WAVE, 9,
	0, PC_PITCH, 70,
	0, PC_ENV_SPEED, -10,
	3, PC_NOTE_UP, 10,
	2, PC_NOTE_CUT, 0,
	0, PATCH_END,
};
// Menu error
const char menuErrorPatch[] =
{
	0,PC_WAVE,5,
	0,PC_ENV_VOL,200,
	0,PC_PITCH,34,
	5,PC_ENV_VOL, 0,
	3,PC_ENV_VOL, 200,
	2,PC_NOTE_CUT,0,
	0,PATCH_END
};
//LINE CLEAR
const char lineClearPatch[] =
{
	0, PC_WAVE, 9,
	0, PC_PITCH, 80,
	0, PC_ENV_SPEED, -5,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_UP, 2,
	1, PC_NOTE_DOWN, 1,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_DOWN, 3,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_DOWN, 4,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_UP, 3,
	0, PATCH_END,
};
//Level Clear
const char levelClearPatch[]  =
{
	0, PC_WAVE, 0,
	0, PC_PITCH, 90,
	0, PC_ENV_SPEED, -4,
	1, PC_NOTE_DOWN, 2,
	1, PC_NOTE_UP, 1,
	1, PC_NOTE_DOWN, 3,
	5, PC_NOTE_UP, 2,
	2, PC_NOTE_DOWN, 6,
	5, PC_NOTE_UP, 7,
	2, PC_NOTE_DOWN, 4,
	5, PC_NOTE_UP, 7,
	2, PC_NOTE_DOWN, 4,
	2, PC_NOTE_UP, 6,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	2, PC_NOTE_UP, 3,
	5, PC_NOTE_UP, 2,
	5, PC_NOTE_CUT, 0,
	0, PATCH_END
};
//dead
const char gameOverPatch[]  =
{
	0, PC_WAVE, 8,
	0, PC_PITCH, 80,
	0, PC_ENV_SPEED, -4,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_UP, 2,
	1, PC_NOTE_DOWN, 3,
	5, PC_NOTE_UP, 2,
	5, PC_NOTE_DOWN, 6,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_DOWN, 4,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_DOWN, 3,
	5, PC_NOTE_UP, 3,
	5, PC_NOTE_DOWN, 3,
	5, PC_NOTE_DOWN, 3,
	5, PC_NOTE_UP, 2,
	5, PC_NOTE_DOWN, 1,
	5, PC_NOTE_CUT, 0,
	0,PATCH_END
};
const char freeFallPatch[]  =
{
	0, PC_WAVE, 9,
	0, PC_PITCH, 99,
	0, PC_ENV_SPEED, -6,
	1, PC_NOTE_DOWN, 2,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	5, PC_NOTE_DOWN, 1,
	5, PC_NOTE_DOWN, 1,
	5, PC_NOTE_DOWN, 1,
	5, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 1,
	1, PC_NOTE_DOWN, 2,
	1, PC_NOTE_CUT, 0,
	0, PATCH_END,
};
const patch_t patches[] = 
{
	{0,  10,(uint8_t*) synthVibraphonePatch, 0, 256},
	{0,  11,(uint8_t*) bassPatch, NUM_ELEMENTS_BASS, NUM_ELEMENTS_BASS},
	{0,  12,(uint8_t*) bassPatch, NUM_ELEMENTS_PIANO, NUM_ELEMENTS_PIANO},
	{0, 0,(uint8_t*) rotatePatch, 0, 256},
	{0, 0,(uint8_t*) menuMove, 0, 256},
	{1, 0,(uint8_t*) menuErrorPatch, 0, 256},
	{0, 0,(uint8_t*) lineClearPatch, 0, 256},
	{0, 0,(uint8_t*) levelClearPatch, 0, 256},
	{0, 0,(uint8_t*) gameOverPatch, 0, 256},
	{0, 0,(uint8_t*) freeFallPatch, 0, 256},		
};
int getNumberOfPatches()
{
	return sizeof(patches) / sizeof(patch_t);
}