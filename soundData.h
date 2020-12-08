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
*  soundData.c/h. PCM samples for uChip Tetris.
*/
#ifndef SOUNDDATA_H_
#define SOUNDDATA_H_

#define NUM_ELEMENTS_VIB 256
#define SPS_VIB 256
extern const char data_vib[NUM_ELEMENTS_VIB];
#define NUM_ELEMENTS_BASS 7866
#define SPS_BASS 30000
extern const char data_bass[NUM_ELEMENTS_BASS];
#define NUM_ELEMENTS_PIANO 32927
#define SPS_PIANO 30000
extern const char data_piano[NUM_ELEMENTS_PIANO];




#endif /* SOUNDDATA_H_ */