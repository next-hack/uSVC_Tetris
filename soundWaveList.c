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
*  soundWaveList.c/h. Sound Wave List for uChip Tetris. These are used for the audio engine, and they are selected using PC_WAVE command in patches.c
*/
#include "main.h"
const soundWave_t soundWaves[] = 
{ 
	{
		.length = 256,
		.wData = (int8_t *)  sineWave,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  sawToothWave,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  triangleWave,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  squareWave25,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  squareWave50,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  squareWave75,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  sineDistoWave1,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  sineDistoWave2,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  sineDistoWave3,
		.sps = 30000
	},
	{
		.length = 256,
		.wData = (int8_t *)  squareWave50Filtered,
		.sps = 30000
	},	
	// end of default sounds
	{
		.length = 256,
		.wData = (int8_t *)  data_vib,
		.sps = SPS_VIB
	},	
	{
		.length = NUM_ELEMENTS_BASS,
		.wData = (int8_t *)  data_bass,
		.sps = SPS_BASS
	},	
	{
		.length = NUM_ELEMENTS_PIANO,
		.wData = (int8_t *)  data_piano,
		.sps = SPS_PIANO	
	}
};
int getNumberOfSoundWaves()
{
	return sizeof(soundWaves) / sizeof (soundWave_t);
}