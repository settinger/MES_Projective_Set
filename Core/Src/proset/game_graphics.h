/*
 * display.h
 *
 *  Created on: Apr 23, 2022
 *      Author: Sam
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "stdio.h"
#include <stdbool.h>

#define LCD_FRAME_BUFFER_LAYER0 (LCD_FRAME_BUFFER + 0x130000)
#define LCD_FRAME_BUFFER_LAYER1 LCD_FRAME_BUFFER

#define BOARD_WIDTH 240
#define BOARD_HEIGHT 320

#define CARD_UNIT     22
#define CARD_PADDING   5
#define CARD_MARGIN    8
#define CARD_HIGHLIGHT 5

#define CARD_WIDTH   (2*CARD_UNIT + 2*CARD_PADDING)
#define CARD_HEIGHT  (3*CARD_UNIT + 2*CARD_PADDING)

#define DOT_RADIUS     7

#define BOARD_HALFWIDTH (BOARD_WIDTH >> 1)
#define BOARD_HALFHEIGHT (BOARD_HEIGHT >> 1)
#define CARD_HALFUNIT (CARD_UNIT >> 1)
#define CARD_HALFWIDTH (CARD_WIDTH >> 1)
#define CARD_HALFHEIGHT (CARD_HEIGHT >> 1)

// Card coordinates:
#define COLUMN_1 (BOARD_HALFWIDTH - 2*CARD_MARGIN - CARD_WIDTH)
#define COLUMN_2 (BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH)
#define COLUMN_3 (BOARD_HALFWIDTH)
#define COLUMN_4 (BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH)
#define COLUMN_5 (BOARD_HALFWIDTH + 2*CARD_MARGIN + CARD_WIDTH)

#define ROW_1    (CARD_MARGIN + CARD_HALFHEIGHT)
#define ROW_2    (3*CARD_MARGIN + CARD_HEIGHT + CARD_HALFHEIGHT)
#define ROW_3    (5*CARD_MARGIN + 2*CARD_HEIGHT + CARD_HALFHEIGHT)

// Return true if touch at (tx, ty) lands on card centered at (x0, y0)
#define CARDHIT(x0, y0, tx, ty) (((x0 - CARD_HALFWIDTH - CARD_MARGIN) < tx) && \
                                 ((x0 + CARD_HALFWIDTH + CARD_MARGIN) > tx) && \
                                 ((y0 - CARD_HALFHEIGHT - CARD_MARGIN) < ty) && \
                                 ((y0 + CARD_HALFHEIGHT + CARD_MARGIN) > ty))

void prepareDisplay(void);
void clearScreen(void);

void drawCard(uint16_t x, uint16_t y, int value, bool selected);
void drawRoundedCard(uint16_t x, uint16_t y);
void drawTable(void);
void drawGameTime(uint32_t);
void drawGameSets(uint16_t sets);

#endif /* SRC_DISPLAY_H_ */
