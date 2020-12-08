/*
*  uChip Tetris, another Tetris clone, this time for uChip Simple VGA Console (uSVC)
*  Copyright 2019-2020 Nicola Wrachien (next-hack.com)
*
*  This game demonstrates the capabilities of the Bitmapped Mode, allowing to show up to
*  256 colors (limitations apply).
*
*  TODO: a lot!
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
*  main.c/h. Main file for initialization and USB input.
*/
#include "main.h"
#include "GameImages.h"
#include "TetrisBlocks.h"
#include <stdlib.h>
 int main(void) 
{	
	// start usvc
	initUsvc(patches);
	// play the song
	startSong(mainTheme);
	uint32_t lastTime = millis();
	int16_t c = -1;
	uint8_t testLed = 0;
	tetrisInit();
	uint16_t keyBuffer[6];
	uint16_t oldKey = 0;
	while (1)
	{
		uint32_t timeNow = millis();
		waitForVerticalBlank();
		soundEngine();
		if (usbHidBootKeyboardIsInstalled())
		{
			c = usbGetKey();
		}
		else if (usbHidGenericGamepadIsInstalled())
		{
			if (oldKey != keyBuffer[0])
				c = keyBuffer[0];
			else
				c  = 0;
			oldKey = keyBuffer[0];
		}
		else
		{
			c = 0;
		}
		tetrisTask(c);

		if (timeNow - lastTime > 1000UL)
		{
			setLed(testLed);
			testLed = 1 - testLed;
			lastTime = timeNow;
		}		
		#if DISABLE_USB == 0
		do
		{
			usbHostTask();
			if (usbHidBootKeyboardIsInstalled())
			{
				usbKeyboardPoll();
				usbGetCurrentAsciiKeyboardStateEx(keyBuffer);
			}
			if (usbHidGenericGamepadIsInstalled())
			{
				gamePadState_t gps;
				usbHidGenericGamepadPoll();
				// gamepad to keyboard conversion
				getCurrentGamepadState(&gps);
				//
				keyBuffer[0] = 0;
				uint32_t buttons = gps.buttons;
				uint8_t k = 0;
				if (gps.axes[1] == gps.XYZRxMinimum)
						keyBuffer[k++] = 'w';
				else if (gps.axes[1] == gps.XYZRxMaximum)
					keyBuffer[k++] = 's';
				if (gps.axes[0] == gps.XYZRxMinimum)
					keyBuffer[k++] = 'a';
				else if (gps.axes[0] == gps.XYZRxMaximum)
					keyBuffer[k++] = 'd';
				uint8_t key = 0;
				switch (buttons)
				{
					case GP_BUTTON_1:
					case GP_BUTTON_SELECT:		// menu confirm
						key = ' ';
						break;
					case GP_BUTTON_2:
						key = 'w';	// rotate
						break;
					case GP_BUTTON_3:
						key = 'g';	// free fall
						break;						
					case GP_BUTTON_4:	// show ghost
							key = 'h';
						break;
					case GP_BUTTON_L1:
						key = '+'; // increase speed
						break;
					case GP_BUTTON_R1: // decrease speed
						key = '-';
						break;
					case GP_BUTTON_START:
						key = 'm';		// go to menu
						break;
				}
				keyBuffer[k] = key;
			}
		} while (getCurrentScanLineNumber() < 523);
		#endif
	}
}
