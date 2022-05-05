/*
 * display.h
 *
 *  Created on: Apr 23, 2022
 *      Author: Sam
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "stdio.h"

#define LCD_FRAME_BUFFER_LAYER0 (LCD_FRAME_BUFFER + 0x130000)
#define LCD_FRAME_BUFFER_LAYER1 LCD_FRAME_BUFFER

#define BOARD_WIDTH 240
#define BOARD_HEIGHT 320

#define CARD_UNIT     22
#define CARD_PADDING   5
#define CARD_MARGIN    8

#define CARD_WIDTH   (2*CARD_UNIT + 2*CARD_PADDING)
#define CARD_HEIGHT  (3*CARD_UNIT + 2*CARD_PADDING)

#define DOT_RADIUS     7

#define BOARD_HALFWIDTH (BOARD_WIDTH >> 1)
#define BOARD_HALFHEIGHT (BOARD_HEIGHT >> 1)
#define CARD_HALFUNIT (CARD_UNIT >> 1)
#define CARD_HALFWIDTH (CARD_WIDTH >> 1)
#define CARD_HALFHEIGHT (CARD_HEIGHT >> 1)

void prepareDisplay(void);

void drawCard(uint16_t x, uint16_t y);
void drawRoundedCard(uint16_t x, uint16_t y);

#endif /* SRC_DISPLAY_H_ */
