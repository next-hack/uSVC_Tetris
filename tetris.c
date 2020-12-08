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
*  tetris.c/h. the game engine/logic.
*
*  Note: this game could (and possibly should) have been done in one of the various tile_mode. However, since the vertical block size is 1, 
*  It allows for smooth showing of the image. Furthermore in the future we could add smooth block falling.
*/
#include "usvc_kernel/usvc_kernel.h"
#include "usvc_config.h"
#include "main.h"
#include "GameImages.h"
#include "TetrisBlocks.h"
#include <stdlib.h>
//
#define GAME_STATE_NOT_STARTED 0
#define GAME_STATE_STARTED 1
#define GAME_STATE_STAGE_CLEAR 2
#define GAME_STATE_GAMEOVER 255
#define TETRIS_SIZE_X 10
#define TETRIS_SIZE_Y 20
#define TETRIS_BLOCK_SIZE_Y 8	
#define TABLE_START_X 7 // position of the table in terms of blocks	
#define NEXT_TETROMINO_WIDTH 5
#define BORDER_THICKNESS_Y 8
#define NEXT_TETROMINO_Y 10
#define NEXT_TETROMINO_X 14	
#define MAX_TETROMINOS 7
#define TETRIS_DEBUG 0
#define TETRIS_SOUND_NO_FX 1
#define TETRIS_SOUND_NO_MUSIC 2
#define PIC_HEIGHT 80
#define GETBLOCK(x, y) (((x) & 1) ? tetrisTable[y][(x) >> 1] >> 4 :  tetrisTable[y][(x) >> 1] & 0xF)
#define SETBLOCK(x, y, b) do {tetrisTable[y][(x) >> 1] =  ((x) & 1) ?  ((b) << 4) |  (tetrisTable[y][((x) >> 1)] & 0xF) :  ((tetrisTable[y][(x) >> 1] & 0xF0) | (b)); } while(0)
//
#define TETROMINO_GHOST_BM 0x80
//
#define TABLE_Y_OFFSET (SCREEN_SIZE_Y - BORDER_THICKNESS_Y -  TETRIS_SIZE_Y * TETRIS_BLOCK_SIZE_Y)
#define TETRIS_FREEFALL_SPEED 256

#define MENU_MODE 0
#define MENU_SOUND 1
#define MENU_DIFFICULTY 2
#define MENU_START_NEW_GAME 3
#define MENU_RESET_CONSOLE 4
#define MENU_DO_NOT_START_NEW_GAME 5
#define MENU_CONFIRM_START_NEW_GAME 6
#define MENU_DO_NOT_RESET 7
#define MENU_CONFIRM_RESET 8
//
#define TETRIS_MODE_LINES 0
#define TETRIS_MODE_TIME 1
#define TETRIS_MODE_FREE_GAME 2
#define TETRIS_GAME_TIMEOUT 300000UL
//
#define TETRIS_DIFFICULTY_USER 3
#define MAX_BLOCK 15	// There are 7 + 7 + 1 blocks (including ghost) on the table!
void drawMenu(uint8_t updateOnly);
// the tetris text.
const uint8_t tetrisText [25] =
{	0b00000000,
	0b00000000,
	0b00000000,
	0b01001000,
	0b01010100,
	0b00100100,
	0b00000000,
	0b01111100,
	0b00000000,
	0b00101100,
	0b01010000,
	0b01111100,
	0b00000000,
	0b01000000,
	0b01111100,
	0b01000000,
	0b00000000,
	0b01000100,
	0b01010100,
	0b01111100,
	0b00000000,
	0b01000000,
	0b01111100,
	0b01000000,
	0b00000000
};
const uint8_t tetrisDifficultyModes[][32] = {"Easy", "Normal", "Difficult", "User In-Game Speed"};
const uint8_t tetrisModes[][32] = {"Lines","Timed", "Free game"};
const uint8_t tetrisSoundModes[][32] = {"Music + FX","Music Only", "FX only", "None"};
void playFx(uint8_t patchNo, uint8_t vol, uint8_t retrig);


	

#ifdef TETRIS_DEBUG
	static volatile char tetrisDebugBuffer[40];
	#define TETRIS_TRACE(...) do { snprintf((char*)tetrisDebugBuffer,sizeof(tetrisDebugBuffer),__VA_ARGS__); addLineAtBottom( (char *) tetrisDebugBuffer, 1, 0);}while(0)
#else
	#define TETRIS_TRACE(x) do{}while(0)
#endif
uint8_t tetrisTable[TETRIS_SIZE_Y][TETRIS_SIZE_X / 2];
static char tmpBuf[40];		// buffer for some display operations

typedef struct
{
	uint8_t blocks_0[2][4];	
	uint8_t w;
	uint8_t h;
} tetromino_t;
const tetromino_t tetrominos[7]= 
{
	// I
	{
		{
			{5, 5, 5, 5},
			{0, 0, 0, 0},
		},
		4,
		1		
	},
	// J
	{
		{
			{6, 6, 6, 0},
			{0, 0, 6, 0},
		},
		3,
		2
	},
	// L
	{
		{
			{2, 2, 2, 0},
			{2, 0, 0, 0},
		},
		3,
		2		
	},
	// O
	{
		{			
			{3, 3, 0, 0},
			{3, 3, 0, 0},
		},
		2,
		2
	},
	// S
	{
		{
			{0, 4, 4, 0},
			{4, 4, 0, 0},
		},
		3,
		2
	},
	// T
	{
		{
			{7, 7, 7, 0},
			{0, 7, 0, 0},
		},
		3,
		2
	},
	// Z
	{
		{
			{1, 1, 0, 0},
			{0, 1, 1, 0},			
		},
		3,
		2
	}
};
void drawTable();
typedef enum
{
	TETRIS_STATE_NEW_GAME = 0,
	TETRIS_STATE_NEW_TETROMINO,
	TETRIS_STATE_TETROMINO_POSITIONING,
	TETRIS_STATE_TETROMINO_FREEFALL,
	TETRIS_STATE_CHECK_FOR_FILLED_ROWS,
	TETRIS_STATE_ANIM_ROW_REMOVAL,
	TETRIS_STATE_CHECK_GRAVITY,
	TETRIS_STATE_GAME_OVER,
	TETRIS_STATE_MENU,
	TETRIS_STATE_CONFIRM_NEW_GAME,
	TETRIS_STATE_NEW_LEVEL,
	TETRIS_WAIT_FOR_NEW_LEVEL,
	TETRIS_STATE_CONFIRM_RESET
} tetrisState_t;
typedef struct  
{
	tetrisState_t state;
	tetrisState_t oldState;	//when entering a menu
	uint8_t mode;
	uint8_t difficulty;
	uint8_t level;
	uint8_t menuMode;
	uint8_t sound;
	uint16_t goalLines;
	uint8_t menuDifficulty;
	uint16_t lines;
	uint8_t gameState;
	uint8_t speed;
	uint32_t time;
	uint32_t menuTime;
	uint8_t menuItemSelected;
	uint32_t score;
	uint32_t highscore;
	uint8_t currentTetromino;
	uint8_t nextTetromino;
	int8_t currentTetrominoX;
	int16_t currentTetrominoY;		// we use 8.8 floating point so that we have the speed possibility.
	uint8_t currentTetrominoRotation;
	uint8_t removeAnimation;
	uint32_t filledRows;
	uint8_t speedIncrement;		
	uint8_t showGhost;
	uint8_t gravityType;	
} tetris_t;
tetris_t tetris;
extern uint8_t paletteIndexTable[];
void putPictureAtBlock(const uint32_t * pictureData, uint16_t sourceW, int xDestBlock, int yDestBlock, int w, int h, uint32_t *paletteData, uint32_t paletteEntries, uint32_t* paletteIdxTable, uint16_t startPalette)
{
	// we need to copy the actual picture data (i.e. pixels), the palette data, and the index table.
	if (w == 0 || h == 0)
		return;
	uint32_t paletteIndexWord = 0x01010101 * (startPalette >> 2);
	for (int y = 0; y < h; y++)
	{
		uint16_t *s1, *d1;
		s1 = &((uint16_t*) pictureData)[y * (sourceW >> 3)];
		d1 = &((uint16_t*) pixels)[(y + yDestBlock) * (SCREEN_SIZE_X >> 3) + xDestBlock];
		uint8_t *s2, *d2;
		s2 = &((uint8_t*) paletteIdxTable)[y * (sourceW >> 3)];
		d2 = &((uint8_t*) paletteIndexTable)[(y + yDestBlock) * (SCREEN_SIZE_X >> 3) + xDestBlock];
		for (int x = 0; x < w; x++)
		{
			*d1++ = *s1++;
			// palette index
			*d2++ = paletteIndexWord + *s2++;
		}
	}
	// finally copy all the palette entries. Normally, this is not required.
	uint32_t *s, *d;
	if (paletteData)
	{
		int n = min(paletteEntries, sizeof(palette) / 4);
		s = paletteData;
		d = (uint32_t*) palette + startPalette;
		for (int i = 0; i < n; i++)
		*d++ = *s++;
	}
}
void putTile(uint8_t * tileData, int tileNum, int x, int y, uint8_t* paletteIdxTable, uint16_t startPalette)
{
	uint8_t paletteIndex = startPalette >> 2;
	uint16_t *s1, *d1;
	s1 = &((uint16_t*) tileData)[tileNum * TILE_SIZE_Y];
	d1 = &((uint16_t*) pixels)[x + y * (SCREEN_SIZE_X >> 3)];
	uint8_t *s2, *d2;
	s2 = &((uint8_t*) paletteIdxTable)[tileNum * TILE_SIZE_Y];
	d2 = &((uint8_t*) paletteIndexTable)[x + y * (SCREEN_SIZE_X >> 3)];
	for (int i = 0; i < TILE_SIZE_Y; i++)
	{
		*d1 = *s1;
		d1 += SCREEN_SIZE_X >> 3;
		s1 ++;
		// palette index
		*d2 = paletteIndex + *s2;
		d2 += SCREEN_SIZE_X >> 3;
		s2 ++;
	}
}
void drawBorders()
{
//	drawPicture((uint32_t*) pic0_data, sizeof(pic0_data) / 4, (uint32_t*) pic0_palettes, PIC0_PALETTE_ENTRIES, (uint32_t*) pic0_paletteIndexTable, PIC0_PALETTE_INDEX_TABLE_SIZE, RESERVED_PALETTE_ENTRIES + (((TETRISBLOCKS_PALETTE_ENTRIES + 3) >> 2) << 2));
//	putPictureAtBlock(pic0_data, 80, 26, 100, 10, 88, (uint32_t*) pic0_palettes, PIC0_PALETTE_ENTRIES, (uint32_t*) pic0_paletteIndexTable, RESERVED_PALETTE_ENTRIES + (((TETRISBLOCKS_PALETTE_ENTRIES + 3) >> 2) << 2));

	for (int y = 0; y < 25; y ++)
	{
		for (int x = 0; x < 40; x++)
			putTile((uint8_t*) tetrisBlocks_data, 15, x, y <<3, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
	}
	for (int y = 0; y < sizeof(tetrisText); y++)
	{
		for (int x = 0; x < 8; x++)
		{
			int xx = 1  << (7 - x);
			int color = (tetrisText[y] & xx) ? (y < 6 ? 0 : y >> 2) : 15;
			putTile((uint8_t*) tetrisBlocks_data, color, x, y <<3, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);

		}
	}
	drawTable();
	printText(NULL, "Next:",21,12, 1,0);	
	printText(NULL, "Progress:",28,12, 1,0);
	
//	showImage(3,40);
}
void showImage(uint8_t imageNumber, int height)
{
	// clear image area
	for (int y = 14; y < 24; y++)
	{
		for (int x = 28; x < 38; x++ )
		{
			putTile((uint8_t*)tetrisBlocks_data, 23, x, y * 8, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
		}		
	}
	// draw only the portion to be shown
	putPictureAtBlock(picData[imageNumber], 80, 28, 112, 10, height, (uint32_t*) picPalettes[imageNumber], paletteEntries[imageNumber], (uint32_t*) picIndexTables[imageNumber], RESERVED_PALETTE_ENTRIES + (((TETRISBLOCKS_PALETTE_ENTRIES + 3) >> 2) << 2));
}
void tetrisInit()
{
	// first 4 colors for print and black.
	palette[0b0000] =  BICOLOR(0,0,0, 0,0,0);
	palette[0b0001] =  BICOLOR(0,0,0, 7,7,3);
	palette[0b0100] =  BICOLOR(7,7,3, 0,0,0);
	palette[0b0101] =  BICOLOR(7,7,3, 7,7,3);
	// cyan
	palette[0b0010] =  LORES_COLOR(0, 7, 3);
	// blue
	palette[0b0011] =  LORES_COLOR(0, 0, 3);
	// orange
	palette[0b0110] =  LORES_COLOR(7, 3, 0);
	// yellow
	palette[0b0111] =  LORES_COLOR(7, 7, 0);
	// green
	palette[0b1000] =  LORES_COLOR(0, 7, 0);
	// purple
	palette[0b1001] =  LORES_COLOR(7, 0, 3);
	// red
	palette[0b1010] =  LORES_COLOR(7, 0, 0);
	// dark red
	palette[0b1011] =   LORES_COLOR(4, 0, 0);
	// dark purple
	palette[0b1100] =   LORES_COLOR(3, 0, 1);
	// dark green
	palette[0b1101] =  LORES_COLOR(0, 3, 0);
	// dark bue
	palette[0b1110] =   LORES_COLOR(0, 0, 2);
	palette[0b1111] =  LORES_COLOR(5, 5, 2);
	initPaletteEntries((uint32_t*)tetrisBlocks_palettes, TETRISBLOCKS_PALETTE_ENTRIES, RESERVED_PALETTE_ENTRIES);
	tetris.state = TETRIS_STATE_MENU;
	drawMenu(0);
	tetris.highscore = 0;
	tetris.score = 0;
	tetris.speed = 3;
	tetris.showGhost = 1;
	tetris.level = 1;
	tetris.lines = 0;
	tetris.goalLines = 1;
	tetris.gameState = GAME_STATE_NOT_STARTED;
}
void drawTable()
{
	// this will automatically clear the table where there are no blocks.
	for (int yt = 0; yt < TETRIS_SIZE_Y; yt++)
	{
		for (int xt = 0; xt < TETRIS_SIZE_X; xt++ )
		{
			uint8_t b = GETBLOCK(xt, yt);
			int x = xt + TABLE_START_X;
			if (b == 0)
			{
				putTile((uint8_t*)tetrisBlocks_data, 23, x, yt * TETRIS_BLOCK_SIZE_Y + TABLE_Y_OFFSET, (uint8_t*)  tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);

			}
			else
			{
				putTile((uint8_t*)tetrisBlocks_data, b - 1, x, yt * TETRIS_BLOCK_SIZE_Y + TABLE_Y_OFFSET, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
			}
		}
	}
}
inline void rotate (uint8_t rotation, int x, int y, int xb, int yb , int *xeff, int *yeff, int sizeX, int sizeY)
{
	switch(rotation)
	{
		default:
		case 0:	// 0° rotation
			*xeff = x + xb;
			*yeff = y + yb;
			break;
		case 1:	// 90° rotation
			*xeff = x + yb;
			*yeff = y + sizeX - xb - 1;
			break;
		case 2: // 180° rotation
			*xeff = x + sizeX - xb - 1;
			*yeff = y + sizeY - yb - 1;
			break;
		case 3:	// 270° rotation
			*xeff = x + sizeY - yb - 1;
			*yeff = y + xb;
			break;
	}	
}
void drawTetromino(uint8_t tetNum,  uint8_t rotation, int8_t x, int8_t y)
{
	// this will draw a tetromino, anywhere on the screen. Positions are relative to the top-left table corner.
	// if drawBlack is 1, then the fully 4x2 tetromino will be drawn over the current image, including black squares
	uint8_t ghost = 0;
	if (tetNum & TETROMINO_GHOST_BM)
	{
		// remove ghost bit mask, so that now tetNum actually is the index of the tetromino.
		tetNum &= ~TETROMINO_GHOST_BM;
		ghost = 1;
	}
	uint8_t sizeX = tetrominos[tetNum].w;
	uint8_t sizeY = tetrominos[tetNum].h;
	for (int xb = 0; xb < sizeX; xb++)
	{
		for (int yb = 0; yb < sizeY; yb++)
		{
			int xeff, yeff;
			rotate(rotation, x, y, xb, yb, &xeff, &yeff, sizeX, sizeY);
			uint8_t b = tetrominos[tetNum].blocks_0[yb][xb];
			int xx = xeff + TABLE_START_X;
			if (b)
			{
				if (!ghost)
				{
					putTile((uint8_t*)tetrisBlocks_data, b - 1 , xx , yeff * TETRIS_BLOCK_SIZE_Y + TABLE_Y_OFFSET, (uint8_t*)  tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
				}
				else
				{
					putTile((uint8_t*)tetrisBlocks_data, b - 1 + 16, xx , yeff * TETRIS_BLOCK_SIZE_Y + TABLE_Y_OFFSET, (uint8_t*)  tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);

				}				
			}

		}
	}	
}

uint8_t canPutTetromino(uint8_t tetNum, uint8_t rotation, int8_t x, int8_t y)
{
	// this function tests if the tetromino can be in position x, y, without colliding.
	// tetNum = number of the tetromino
	// rotation = 0: not rotated. 1-3 rotated 90°-180-270. 
	// x,y position. x can be -1 for tetrominos 2-7 rotated by 180°
	// 
	// NOTE: this could be optimized a lot better
	uint8_t sizeX = tetrominos[tetNum].w;
	uint8_t sizeY = tetrominos[tetNum].h;
	for (int xb = 0; xb < sizeX; xb++)
	{
		for (int yb = 0; yb < sizeY; yb++)
		{
			int xeff, yeff;
			rotate(rotation, x, y, xb, yb, &xeff, &yeff, sizeX, sizeY);		
			if (xeff < 0 || xeff >= TETRIS_SIZE_X || yeff < 0 || yeff >= TETRIS_SIZE_Y)
				return 0;
			if (tetrominos[tetNum].blocks_0[yb][xb] && GETBLOCK(xeff, yeff))
				return 0;
		}
	}
	// no collision detected! The block can be put!
	return 1;
}
uint32_t checkFilledLines(void)
{
	uint8_t x, y;
	uint32_t filled = 0;
	for (y = 0; y < TETRIS_SIZE_Y; y++)
	{
		for (x = 0; x < TETRIS_SIZE_X; x++)
		{
			uint8_t b = GETBLOCK(x, y);
			if (!b)		// we found a non filled block
				break;	
		}		
		if (x == TETRIS_SIZE_X)
		{  // set row as filled
			filled |= 1 << y;
		}
	}
	return filled;
}

void markFilledLines(uint32_t filled)
{
	for (int y = 0; y < TETRIS_SIZE_Y; y++)
	{
		if ( (1 << y) & filled)
		{  // set
			memset(&tetrisTable[y], 8 | (8 << 4), TETRIS_SIZE_X / 2 );		// put white blocks...
		}
	}
}
void removeFilledLines(uint32_t filled, uint8_t animation)
{
	animation = animation >> 1;
	if (animation >= TETRIS_SIZE_X / 2 )
	{
		animation = TETRIS_SIZE_X / 2;
	}
	for (int y = 0; y < TETRIS_SIZE_Y; y++)
	{
		for (int x = 0; x < animation; x++)
		{
			if ( (1 << y) & filled)
			{  // set
				int	b = GETBLOCK(TETRIS_SIZE_X / 2 + x , y);
				if (b == MAX_BLOCK || b == 0)
					b = 0;	
				else 
					b++;
				SETBLOCK(TETRIS_SIZE_X / 2 + x , y, b);
				SETBLOCK(TETRIS_SIZE_X / 2 - 1 - x, y, b);
			}			
		}
	}

}
void drawGhost(int8_t tetNum, uint8_t rotation, int8_t x, int8_t startY)
{
	// not optimized but heck, we have a lot of time during blanks!
	if (!tetris.showGhost)
	{
		return;
	}
	int y = startY;
	for (y = startY; y < TETRIS_SIZE_Y; y++)
	{
		if (!canPutTetromino(tetNum, rotation, x, y))
		{
			if (y > startY)
			{
				drawTetromino(tetNum | TETROMINO_GHOST_BM, rotation, x, y - 1);
			}
			return ;
		}
	}
	drawTetromino(tetNum | TETROMINO_GHOST_BM, rotation, x, y - 1);
}
uint8_t checkGravity(uint8_t type, uint32_t filled)
{
	int newY = TETRIS_SIZE_Y - 1;
	if (type == 0)
	{
		for (int y = TETRIS_SIZE_Y - 1; y >= 0 ; y--)
		{
			// was the row filled? Then do not copy it! 
			if ( ! ((1 << y) & filled))
			{
				memcpy(&tetrisTable[newY], &tetrisTable[y], TETRIS_SIZE_X / 2);
				newY--;
			}
		}		
		// the remaining rows should be cleared
		if (newY >= 0)
			memset(&tetrisTable[0], 0, (newY + 1) * TETRIS_SIZE_X / 2);
		return 0;
	}
	return 1;
}

void updateSpeedText()
{
	snprintf(tmpBuf,sizeof(tmpBuf),"Speed: %d", tetris.speed);
	printText(NULL, tmpBuf,21,4, 1, 0);	
}
void updateScoresText()
{
	
	snprintf(tmpBuf,sizeof(tmpBuf),"Score: %05lu", tetris.score);
	printText(NULL, tmpBuf,13,1,1,0);
	snprintf(tmpBuf,sizeof(tmpBuf),"High: %05lu", tetris.highscore);
	printText(NULL, tmpBuf,28,1, 1,0);
}
void updateLevelText()
{
	snprintf(tmpBuf,sizeof(tmpBuf),"Level: %02u", tetris.level);
	printText(NULL, tmpBuf,1,1,1,0);
}
void updateLinesText()
{
	if (tetris.mode != TETRIS_MODE_FREE_GAME)
		snprintf(tmpBuf,sizeof(tmpBuf),"Lines: %04u/%u", tetris.lines, tetris.goalLines);
	else
		snprintf(tmpBuf,sizeof(tmpBuf),"Lines: %04u", tetris.lines);
	printText(NULL, tmpBuf,21,6,1,0);
}
void playFx(uint8_t patchNum, uint8_t vol, uint8_t retrig)
{
	if (!(tetris.sound & TETRIS_SOUND_NO_FX))
	{
		triggerFx(patchNum, vol, (retrig ? FX_FLAGS_RETRIG : 0), 0x10000);
	}
}
void updateTimeText()
{
	static uint32_t oldTime = 0;
	uint32_t time = (millis() - tetris.time) / 100;
	if (time != oldTime)
	{
		oldTime = time;
		if (tetris.mode == TETRIS_MODE_TIME)
		{
			if (time > TETRIS_GAME_TIMEOUT / 100)
				time = 0;
			else
				time = (TETRIS_GAME_TIMEOUT / 100 - time);
		}
		uint32_t minutes = time / (600);
		uint32_t seconds = (time / 10) % 60;
		uint32_t decs = time % 10;
		snprintf(tmpBuf,sizeof(tmpBuf),"Time: %02lu:%02lu.%01lu", minutes, seconds, decs);
		printText(NULL, tmpBuf,21,8,1,0);		
	}
}
void drawGravityAndShowGhost()
{
	snprintf(tmpBuf,sizeof(tmpBuf),"Show ghost: %s", tetris.showGhost ? "yes" : "no ");
	printText(NULL, tmpBuf,21,19,1,0);	
	snprintf(tmpBuf,sizeof(tmpBuf),"Gravity: %s", tetris.gravityType ? "naive" : "realistic ");
	printText(NULL, tmpBuf,21,19, 1,0);

}
void drawNextTetromino()
{
	// warning: NEXT_TETROMINO_X is in terms of square blocks with respect to the tetris table start X position!
	for (int xx = 0; xx < NEXT_TETROMINO_WIDTH; xx++ )
		for (int yy = 0 ; yy < TETRIS_BLOCK_SIZE_Y * 2; yy+=8)
			putTile((uint8_t*)tetrisBlocks_data, 23, xx + TABLE_START_X + NEXT_TETROMINO_X, (yy + NEXT_TETROMINO_Y * TETRIS_BLOCK_SIZE_Y + TABLE_Y_OFFSET), (uint8_t*)  tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);			
	drawTetromino(tetris.nextTetromino, 0 , 1 +  NEXT_TETROMINO_X , NEXT_TETROMINO_Y);
}

void putTetromino(uint8_t tetNum, uint8_t rotation, uint8_t x, uint8_t y)
{
	// this will put a bock regardless if it is possible
	uint8_t sizeX = tetrominos[tetNum].w;
	uint8_t sizeY = tetrominos[tetNum].h;
	for (int xb = 0; xb < sizeX; xb++)
	{
		for (int yb = 0; yb < sizeY; yb++)
		{
			int xeff, yeff;
			rotate(rotation, x, y, xb, yb, &xeff, &yeff, sizeX, sizeY);
			if (xeff >= 0 && xeff < TETRIS_SIZE_X && yeff >= 0 && yeff < TETRIS_SIZE_Y)
			{
				uint8_t b = tetrominos[tetNum].blocks_0[yb][xb];
				if (b)
					SETBLOCK( xeff, yeff, b);
			}
		}
	}	
}
// uint32_t rnd()
// {
// 	const uint32_t a = 16807;
// 	const uint32_t m = 2147483647;
// 	tetrominoSeed = (a * tetrominoSeed + 1) & m;
// 	return tetrominoSeed;	
// }
uint8_t getRandomTetromino()
{
	return rand() % MAX_TETROMINOS;	
}
void setRandomTetrominoSeed(uint32_t s)
{
	for (int i = 0; i < 32768 >> 2; i++)
	{
		s += *((uint32_t*) 0x20000000 + i);
	}
	srand(s);
}
void updateProgress(uint8_t force)
{
	static uint8_t oldHeight = 0;
	int32_t height = 0;
	switch (tetris.mode)
	{
		case TETRIS_MODE_LINES:
		case TETRIS_MODE_TIME:
			if (tetris.goalLines != 0)
			{
				height = PIC_HEIGHT * tetris.lines / tetris.goalLines;
			}
			if (height > PIC_HEIGHT)
			{
				height = PIC_HEIGHT;
			}
			break;
		case TETRIS_MODE_FREE_GAME:
			height = 80;
			break;
	}
	if (force || oldHeight != height)
	{
		showImage(tetris.level % getNumberOfImages(), height);	
	}
	oldHeight = height;
}
void drawMenu(uint8_t updateOnly)
{
	// first, fill the whole section of bricks
	if (!updateOnly)
	{
		for (int y = 0; y < 25; y ++)
		{
			for (int x = 0; x < 40; x++)
				putTile((uint8_t*) tetrisBlocks_data, 15, x, y <<3, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
		}
		// draw horizontal tetris text
		for (int x = 0; x < sizeof(tetrisText); x++)
		{

			for (int y = 0; y < 8; y++)
			{
				int yy = 1  << (7 - y);
				int xx = sizeof(tetrisText) - x;
				int color = (tetrisText[xx] & yy) ? (xx < 6 ? 0 : xx >> 2) : 15;
				putTile((uint8_t*) tetrisBlocks_data, color, x + 7, y << 3, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
			}
		}	
		switch (tetris.gameState)
		{
			case GAME_STATE_NOT_STARTED:
				printText(NULL, "Game not started", 12, 8,  1, 0 );	
				break;
			case GAME_STATE_STARTED:
				printText(NULL, "Game paused.",14, 8,  1, 0 );
				printText(NULL, "Press m/start to return to game!",4, 9,  1, 0 );
				break;
			case GAME_STATE_GAMEOVER:
				printText(NULL, "Game over!",15, 8,  1, 0 );
				printText(NULL, "Select <new game> to start",7, 9,  1, 0 );
				break;
		}
	}
	else
	{ 
		for (int y = 10; y < 25; y ++)
		{
			for (int x = 0; x < 40; x++)
				putTile((uint8_t*) tetrisBlocks_data, 15, x, y <<3, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
		}	
	}
	// now write the settings	
	snprintf(tmpBuf,sizeof(tmpBuf),"Mode: <%s>",tetrisModes[tetris.menuMode]);
	printText(NULL, tmpBuf,(40 - strlen(tmpBuf)) / 2, 11,  tetris.menuItemSelected == MENU_MODE ? 0 : 1 , tetris.menuItemSelected == MENU_MODE ? 1 : 0 );
	//
	snprintf(tmpBuf,sizeof(tmpBuf),"In game Sounds: <%s>",tetrisSoundModes[tetris.sound]);
	printText(NULL, tmpBuf,(40 - strlen(tmpBuf)) / 2, 13,  tetris.menuItemSelected == MENU_SOUND ? 0 : 1 , tetris.menuItemSelected == MENU_SOUND ? 1 : 0 );
	//
	snprintf(tmpBuf,sizeof(tmpBuf),"Difficulty: <%s>",tetrisDifficultyModes[tetris.menuDifficulty]);
	printText(NULL, tmpBuf,(40 - strlen(tmpBuf)) / 2, 15,  tetris.menuItemSelected == MENU_DIFFICULTY ? 0 : 1 , tetris.menuItemSelected == MENU_DIFFICULTY ? 1 : 0 );		
	//
	snprintf(tmpBuf,sizeof(tmpBuf),"Start New Game");
	printText(NULL, tmpBuf,(40 - strlen(tmpBuf)) / 2, 17,  tetris.menuItemSelected == MENU_START_NEW_GAME ? 0 : 1 , tetris.menuItemSelected == MENU_START_NEW_GAME ? 1 : 0 );
	//
	snprintf(tmpBuf,sizeof(tmpBuf),"Reset Console to Game Loader");
	printText(NULL, tmpBuf,(40 - strlen(tmpBuf)) / 2, 21,  tetris.menuItemSelected == MENU_RESET_CONSOLE ? 0 : 1 , tetris.menuItemSelected == MENU_RESET_CONSOLE ? 1 : 0 );	
	//
	if (tetris.state == TETRIS_STATE_CONFIRM_NEW_GAME)
	{
		snprintf(tmpBuf,sizeof(tmpBuf),"Confirm Start New Game?");
		printText(NULL, tmpBuf,(40 - strlen(tmpBuf)) / 2, 17,  1 , 0 );
		printText(NULL, "No!",14, 19,  tetris.menuItemSelected == MENU_DO_NOT_START_NEW_GAME ? 0 : 1 , tetris.menuItemSelected == MENU_DO_NOT_START_NEW_GAME ? 1 : 0 );		
		printText(NULL, "Yes!",23, 19,  tetris.menuItemSelected == MENU_CONFIRM_START_NEW_GAME ? 0 : 1 , tetris.menuItemSelected == MENU_CONFIRM_START_NEW_GAME ? 1 : 0 );
	}
	if (tetris.state == TETRIS_STATE_CONFIRM_RESET)
	{
		snprintf(tmpBuf,sizeof(tmpBuf),"Confirm Reset Console to Game Loader?");
		printText(NULL, tmpBuf,(40 - strlen(tmpBuf)) / 2, 21,  1 , 0 );
		printText(NULL, "No!",14, 23,  tetris.menuItemSelected == MENU_DO_NOT_RESET ? 0 : 1 , tetris.menuItemSelected == MENU_DO_NOT_RESET ? 1 : 0 );
		printText(NULL, "Yes!",23, 23,  tetris.menuItemSelected == MENU_CONFIRM_RESET ? 0 : 1 , tetris.menuItemSelected == MENU_CONFIRM_RESET ? 1 : 0 );
	}


}
void tetrisTask(int16_t key)
{
	if (tetris.state != TETRIS_STATE_MENU && tetris.state != TETRIS_STATE_CONFIRM_NEW_GAME && tetris.state != TETRIS_STATE_CONFIRM_RESET && tetris.state != TETRIS_WAIT_FOR_NEW_LEVEL)
	{
		drawTable();
		if (tetris.gameState == GAME_STATE_STARTED)
		{
			if (tetris.mode == TETRIS_MODE_TIME)
			{
				if (millis() - tetris.time > TETRIS_GAME_TIMEOUT)
				{
					tetris.state = TETRIS_STATE_GAME_OVER;
				}
			}
			updateTimeText();
			updateProgress(0);
		}
	}
	switch (tetris.state)
	{
		case TETRIS_STATE_NEW_GAME:
			tetris.score = 0;
			tetris.level = 0;
		// NOTE: BREAK INTENTIONALLY NOT PLACED!
		case TETRIS_STATE_NEW_LEVEL:
			tetris.gameState = 1;
			tetris.lines = 0;
			if (tetris.difficulty != TETRIS_DIFFICULTY_USER)
			{
				tetris.speed = 3 + 	tetris.difficulty + (tetris.level >> 3);
				if (tetris.speed > 6)
					tetris.speed = 6;
			}
			else
				tetris.speed = 3;	
			tetris.goalLines = 3 * (tetris.level) + 1;
			tetris.time = millis();
			drawBorders();
			memset(tetrisTable, 0, sizeof (tetrisTable));
			if (tetris.level > 4)
			{
				int n = (tetris.level - 4);
				for (int i = 0; i <= n / 8; i++)
				{
					for (int j = 0; j < 8; j++)
					{
						int x = rand() % TETRIS_SIZE_X;
						int b = rand() % 7 + 1;
						SETBLOCK(x,TETRIS_SIZE_Y - i - 1,b);
					}
				}
			}

			tetris.state = TETRIS_STATE_NEW_TETROMINO;
			setRandomTetrominoSeed(millis());
			tetris.nextTetromino = getRandomTetromino();
			updateSpeedText();
			updateScoresText();
			updateLevelText();			
			updateLinesText();
			updateProgress(1);			
			break;
		case TETRIS_STATE_NEW_TETROMINO:
			// generate a new tetromino
			tetris.filledRows = 0;
			tetris.currentTetrominoRotation = 0;
			tetris.currentTetromino = tetris.nextTetromino;
			tetris.nextTetromino = getRandomTetromino();
			tetris.currentTetrominoX = TETRIS_SIZE_X / 2;
			tetris.currentTetrominoY = 0;
			tetris.state = TETRIS_STATE_TETROMINO_POSITIONING;
			tetris.speedIncrement = 0;
			drawNextTetromino();
			
			if (!canPutTetromino(tetris.currentTetromino, tetris.currentTetrominoRotation, tetris.currentTetrominoX, 0))
				tetris.state = TETRIS_STATE_GAME_OVER;
			else
			{
				drawGhost(tetris.currentTetromino,tetris.currentTetrominoRotation ,  tetris.currentTetrominoX, tetris.currentTetrominoY >> 8);	
				drawTetromino(tetris.currentTetromino,tetris.currentTetrominoRotation ,  tetris.currentTetrominoX, tetris.currentTetrominoY >> 8);			
			}
			break;
		case TETRIS_STATE_TETROMINO_POSITIONING:
			{
				// In the previous iteration either being TETRIS_STATE_NEW_BLOCK or TETRIS_STATE_TETROMINO_POSITIONING, we already checked if the tetromino could be placed.
				int16_t oldY = tetris.currentTetrominoY;
				int8_t oldX = tetris.currentTetrominoX;
				uint8_t oldR = tetris.currentTetrominoRotation;
				tetris.currentTetrominoY += tetris.speedIncrement ? TETRIS_FREEFALL_SPEED : 1 << tetris.speed;
				if (!canPutTetromino(tetris.currentTetromino, tetris.currentTetrominoRotation, tetris.currentTetrominoX, tetris.currentTetrominoY >> 8))
				{
					// we cannot move down any further. Then put the tetromino in the previous y position and ask for a new block. 
					tetris.currentTetrominoY = oldY;
					putTetromino(tetris.currentTetromino,tetris.currentTetrominoRotation ,  tetris.currentTetrominoX, tetris.currentTetrominoY >> 8);
					tetris.state = TETRIS_STATE_CHECK_FOR_FILLED_ROWS;
				}
				else
				{
					tetris.speedIncrement = 0;
					switch (key)
					{
						case 'a':
							// left
							tetris.currentTetrominoX--;
							playFx(MENU_MOVE_PATCH, 255, 1);
							break;
						case 's':
							// make the block fall by one line.
							playFx(MENU_MOVE_PATCH, 255, 1);						
							tetris.speedIncrement = 1; 
						break;
						// cheat to test levels...
						case 'l':
							tetris.level++;
							updateLevelText();
							break;
						case 'd':
							// right
							playFx(MENU_MOVE_PATCH, 255, 1);
							tetris.currentTetrominoX++;
							break;
						case 'w':
							// rotate
							playFx(ROTATE_PATCH, 255, 1);
							tetris.currentTetrominoRotation = (tetris.currentTetrominoRotation + 1) & 0x3;
							break;
						case 'g':
							// free fall
							playFx(FREE_FALL_PATCH, 255, 1); 
							tetris.state = TETRIS_STATE_TETROMINO_FREEFALL;
							break;
						case '+':
							if (tetris.speed < 6)
							{
								tetris.speed++;
								updateSpeedText();								
							}
							break;
						case 'h':
							tetris.showGhost = 1- tetris.showGhost;
							break;
						case '-':
							if (tetris.speed > 0)
							{
								tetris.speed--;
								updateSpeedText();
								// print speed.	
							}
							break;
					}				
					if (!canPutTetromino(tetris.currentTetromino, tetris.currentTetrominoRotation, tetris.currentTetrominoX, tetris.currentTetrominoY >> 8))
					{
						// restore previous state
						tetris.currentTetrominoX = oldX;
						tetris.currentTetrominoRotation = oldR;
					}
				}
				drawGhost(tetris.currentTetromino,tetris.currentTetrominoRotation ,  tetris.currentTetrominoX, tetris.currentTetrominoY >> 8);	
				drawTetromino(tetris.currentTetromino,tetris.currentTetrominoRotation ,  tetris.currentTetrominoX, tetris.currentTetrominoY >> 8);				
			}
			break;
		case TETRIS_STATE_TETROMINO_FREEFALL:
			{
				int16_t oldY = tetris.currentTetrominoY;
				tetris.currentTetrominoY += TETRIS_FREEFALL_SPEED;
				if (!canPutTetromino(tetris.currentTetromino, tetris.currentTetrominoRotation, tetris.currentTetrominoX, tetris.currentTetrominoY >> 8))
				{
					// we cannot move down any further. Then put the tetromino in the previous y position and ask for a new block.
					tetris.currentTetrominoY = oldY;
					putTetromino(tetris.currentTetromino,tetris.currentTetrominoRotation ,  tetris.currentTetrominoX, tetris.currentTetrominoY >> 8);
					tetris.state = TETRIS_STATE_CHECK_FOR_FILLED_ROWS;
				}
				drawTetromino(tetris.currentTetromino,tetris.currentTetrominoRotation ,  tetris.currentTetrominoX, tetris.currentTetrominoY >> 8);
			}
			break;
		case TETRIS_STATE_CHECK_FOR_FILLED_ROWS:
			{
				int n = 0;
				tetris.filledRows = checkFilledLines();
				for (int i = 0; i < 32; i++)
					if ( (1 << i) & tetris.filledRows)
					{
						n++;						
					} 
				switch (n)
				{
					case 1:
						tetris.score += 10;
						break;
					case 2:
						tetris.score += 50;
						break;
					case 3:
						tetris.score += 200;
						break;
					case 4:
						tetris.score += 500;
						break;
				}  
				if (tetris.score > tetris.highscore)
					tetris.highscore = tetris.score;
				tetris.lines += n;
				if (tetris.lines >= tetris.goalLines && tetris.mode != TETRIS_MODE_FREE_GAME)
				{
					tetris.gameState = GAME_STATE_STAGE_CLEAR;
					tetris.filledRows = 0xFFFFF;		// all the table is filled
				}				
				if (tetris.filledRows)
				{
					tetris.state = TETRIS_STATE_ANIM_ROW_REMOVAL;
					markFilledLines( tetris.filledRows);	
					if (tetris.gameState == GAME_STATE_STAGE_CLEAR)		
					{
						playFx(LEVEL_CLEAR_PATCH, 255, 1);
					}		
					else
					{
						playFx(LINE_CLEAR_PATCH, 255, 1);						
					}
					updateProgress(0);
					updateScoresText();						
					updateLinesText();						
				}
				else // no filled rows. Then ask for a new block
					tetris.state = TETRIS_STATE_NEW_TETROMINO;
				tetris.removeAnimation = 0;
				//draw_table();	
			}
			break;
		case TETRIS_STATE_ANIM_ROW_REMOVAL:
			tetris.removeAnimation++;
			removeFilledLines(tetris.filledRows, tetris.removeAnimation);	
			if (tetris.removeAnimation == 20)
			{
				// the first step is always to remove rows in a naive way!
				checkGravity(0, tetris.filledRows); 
				if (tetris.gameState == GAME_STATE_STAGE_CLEAR)
				{
					// clear line
					for (int x = 0; x < 40; x++)
					{
						putTile((uint8_t*) tetrisBlocks_data, 15, x, 12 <<3, (uint8_t*) tetrisBlocks_paletteIndexTable, RESERVED_PALETTE_ENTRIES);
					}
					snprintf(tmpBuf, sizeof(tmpBuf),"Stage %d clear! Press any key!", tetris.level);
					tetris.level++;
					printText (NULL, tmpBuf,(40 - strlen(tmpBuf)) /2, 12,  1, 0);
					tetris.state = TETRIS_WAIT_FOR_NEW_LEVEL;
				}
				else if (!tetris.gravityType)
				{
					tetris.state = TETRIS_STATE_NEW_TETROMINO;
					tetris.filledRows = 0;					
				}
				else
				{				
					tetris.state = TETRIS_STATE_CHECK_GRAVITY;
				}
			}
			break;
		case TETRIS_STATE_CHECK_GRAVITY:
			// we continuously check gravity until no blocks are moved. When no blocks are moved, we check if there are now filled lines.
			if (checkGravity(tetris.gravityType, tetris.filledRows)) // no blocks were moved 
			{
				tetris.state = TETRIS_STATE_CHECK_FOR_FILLED_ROWS;	
				tetris.filledRows = 0;
			}
			break;
		case TETRIS_STATE_GAME_OVER:
			if (tetris.gameState == 1)
			{
				tetris.gameState = GAME_STATE_GAMEOVER;
				playFx(GAME_OVER_PATCH, 255, 1);	
			}
			if ((millis() - tetris.time > TETRIS_GAME_TIMEOUT) && tetris.mode == TETRIS_MODE_TIME)
			{
				printText(NULL, "GAME OVER! (timeout)", 10, 10,  1, 0 );				
			}
			else
				printText(NULL, "GAME OVER!", 15, 10,  1, 0 );
			printText(NULL, "(press any key to start a new game)", 2, 10,  1, 0 );
			if (key != -1 && key != 0)
				tetris.state = TETRIS_STATE_NEW_GAME;
			break;
		case TETRIS_WAIT_FOR_NEW_LEVEL:
			if (key != - 1 && key != 0)
			{
				playFx(MENU_MOVE_PATCH, 255, 1);
				tetris.state = TETRIS_STATE_NEW_LEVEL;
			}
			break;
		case TETRIS_STATE_MENU:
			switch(key)
			{
				case 0:			// to avoid sound!
				case -1:		// to avoid sound!
					break;
				case 'w':
					if (tetris.menuItemSelected > 0)
					{
						tetris.menuItemSelected --;
						playFx(MENU_MOVE_PATCH, 255, 1);
					}
					else
						playFx(MENU_ERROR_PATCH, 255, 1);		
					break;
				case 's':
					if (tetris.menuItemSelected < 4)
					{
						tetris.menuItemSelected ++;
						playFx(MENU_MOVE_PATCH, 255, 1);
					}
					else
						playFx(MENU_ERROR_PATCH, 255, 1);						
					break;
				case 'a':
					switch (tetris.menuItemSelected)
					{
						case MENU_MODE:
							if (tetris.menuMode > 0)
							{
								tetris.menuMode--;
								playFx(MENU_MOVE_PATCH, 255, 1);
							}
							else
								playFx(MENU_ERROR_PATCH, 255, 1);								
						break;
						case MENU_SOUND:
							if (tetris.sound > 0)
							{
								tetris.sound--;
								if (tetris.sound & TETRIS_SOUND_NO_MUSIC)
									stopSong();
								else
									startSong(mainTheme);								
								playFx(MENU_MOVE_PATCH, 255, 1);
							}
							else
								playFx(MENU_ERROR_PATCH, 255, 1);							
						break;
						case MENU_DIFFICULTY:
							if (tetris.menuDifficulty > 0)
							{
								tetris.menuDifficulty--;
								playFx(MENU_MOVE_PATCH, 255, 1);
							}
							else
								playFx(MENU_ERROR_PATCH, 255, 1);								
							break;
						default:
							playFx(MENU_ERROR_PATCH, 255, 1);
							break;							
					}
					break;	
				case 'd':
					switch (tetris.menuItemSelected)
					{
						case MENU_MODE:
							if (tetris.menuMode < 2)
							{
								tetris.menuMode++;
								playFx(MENU_MOVE_PATCH, 255, 1);
							}
							else
								playFx(MENU_ERROR_PATCH, 255, 1);								
							break;
						case MENU_SOUND:
							if (tetris.sound < 3)
							{
								tetris.sound++;
								if (tetris.sound & TETRIS_SOUND_NO_MUSIC)
									stopSong();
								else
									startSong(mainTheme);
								playFx(MENU_MOVE_PATCH, 255, 1);
							}
							else
								playFx(MENU_ERROR_PATCH, 255, 1);
							break;
						case MENU_DIFFICULTY:
							if (tetris.menuDifficulty < 3)
							{
								tetris.menuDifficulty++;
								playFx(MENU_MOVE_PATCH, 255, 1);
							}
							else
								playFx(MENU_ERROR_PATCH, 255, 1);
							break;
						default:
							break;
					}
					break;			
				case ' ':
					if (tetris.menuItemSelected == MENU_START_NEW_GAME)
					{
						tetris.state = TETRIS_STATE_CONFIRM_NEW_GAME;
						tetris.menuItemSelected = MENU_DO_NOT_START_NEW_GAME;
						playFx(MENU_MOVE_PATCH, 255, 1);
					}
					else if (tetris.menuItemSelected == MENU_RESET_CONSOLE)
					{
						tetris.state = TETRIS_STATE_CONFIRM_RESET;
						tetris.menuItemSelected = MENU_DO_NOT_RESET;
						playFx(MENU_MOVE_PATCH, 255, 1);
					}
					break;
				default:
					playFx(MENU_ERROR_PATCH, 255, 1);
			}
			drawMenu(1);
			break;
		case TETRIS_STATE_CONFIRM_NEW_GAME:
			switch (key)
			{
				case ' ':
					if (tetris.menuItemSelected == MENU_DO_NOT_START_NEW_GAME)
					{
						tetris.menuItemSelected = MENU_START_NEW_GAME;
						tetris.state = TETRIS_STATE_MENU;
					}
					else
					{
						tetris.menuItemSelected = MENU_MODE;
						tetris.state = TETRIS_STATE_NEW_GAME;
						tetris.difficulty = tetris.menuDifficulty;
						tetris.mode = tetris.menuMode;
					}
					break;
				case 'a':
					if (tetris.menuItemSelected == MENU_CONFIRM_START_NEW_GAME)
					{
						tetris.menuItemSelected = MENU_DO_NOT_START_NEW_GAME;
						playFx(MENU_MOVE_PATCH, 255, 1);
					}
					else
						playFx(MENU_ERROR_PATCH, 255, 1);						
					break;
				case 'd':
					if (tetris.menuItemSelected == MENU_DO_NOT_START_NEW_GAME)
					{
						tetris.menuItemSelected = MENU_CONFIRM_START_NEW_GAME;
						playFx(MENU_MOVE_PATCH, 255, 1);
					}
					else
						playFx(MENU_ERROR_PATCH, 255, 1);
					break;										
			}
			drawMenu(1);
			break;
		case TETRIS_STATE_CONFIRM_RESET:
			switch (key)
			{
				case ' ':
					if (tetris.menuItemSelected == MENU_DO_NOT_RESET)
					{
						tetris.menuItemSelected = MENU_RESET_CONSOLE;
						tetris.state = TETRIS_STATE_MENU;
					}
					else
					{
						//  reset.
						SCB->AIRCR  = ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |  SCB_AIRCR_SYSRESETREQ_Msk);
						while(1);							
					}
					break;
				case 'a':
					if (tetris.menuItemSelected == MENU_CONFIRM_RESET)
					{
						tetris.menuItemSelected = MENU_DO_NOT_RESET;
						playFx(MENU_MOVE_PATCH, 255, 1);
					}
					else
						playFx(MENU_ERROR_PATCH, 255, 1);
				break;
				case 'd':
				if (tetris.menuItemSelected == MENU_DO_NOT_RESET)
				{
					tetris.menuItemSelected = MENU_CONFIRM_RESET;
					playFx(MENU_MOVE_PATCH, 255, 1);
				}
				else
					playFx(MENU_ERROR_PATCH, 255, 1);
				break;
			}
			drawMenu(1);
			break;
		default:
			playFx(MENU_ERROR_PATCH, 255, 1);
			break;	// do nothing
	}
	// from any time, you can press the menu button and jump to the menu
	if (key == 'm' || key == 'M')
	{
		if (tetris.state != TETRIS_STATE_MENU)
		{
			tetris.oldState = tetris.state;		// backup menu
			drawMenu(0);
			tetris.menuItemSelected = 0;
			tetris.state = TETRIS_STATE_MENU;	
			tetris.menuTime = millis();		
		}
		else
		{
			tetris.time += millis() - tetris.menuTime;		// remove the time we spent in the menu phase
			tetris.state = tetris.oldState ;		// restore ackup
			drawBorders();
			updateTimeText();
			updateSpeedText();
			updateScoresText();
			updateLevelText();						
			updateLinesText();
			updateProgress(1);
			drawNextTetromino();
		}
		// 
	}
}
