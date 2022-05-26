/*
 * display.c
 *
 *  Created on: Apr 23, 2022
 *      Author: Sam
 */

#include <game_graphics.h>
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "stdio.h"
#include <stdbool.h>

/*
 * Board dimensions
 * For now, assume 240x320 pixel display, origin in top left
 */

/* Draw a card centered at (x,y) */
void drawRoundedCard(uint16_t x, uint16_t y) {
  uint16_t xShift = CARD_HALFWIDTH - DOT_RADIUS;
  uint16_t yShift = CARD_HALFHEIGHT - DOT_RADIUS;

  /* Draw the card body fill */
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillCircle(x - xShift, y - yShift, DOT_RADIUS);
  BSP_LCD_FillCircle(x + xShift, y - yShift, DOT_RADIUS);
  BSP_LCD_FillCircle(x - xShift, y + yShift, DOT_RADIUS);
  BSP_LCD_FillCircle(x + xShift, y + yShift, DOT_RADIUS);
  BSP_LCD_FillRect(x - xShift, y - CARD_HALFHEIGHT, CARD_WIDTH - DOT_RADIUS * 2,
  CARD_HEIGHT);
  BSP_LCD_FillRect(x - CARD_HALFWIDTH, y - yShift, CARD_WIDTH,
  CARD_HEIGHT - DOT_RADIUS * 2);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

  /* Draw the fillets (cribbed from BSP_LCD_DrawCircle function) */
  int32_t d;/* Decision Variable */
  uint32_t curx;/* Current X Value */
  uint32_t cury;/* Current Y Value */
  uint16_t Radius = DOT_RADIUS;

  d = 3 - (Radius << 1);
  curx = 0;
  cury = Radius;

  while (curx <= cury) {
    BSP_LCD_DrawPixel((x + curx + xShift), (y - cury - yShift),
    LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - curx - xShift), (y - cury - yShift),
    LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x + cury + xShift), (y - curx - yShift),
    LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - cury - xShift), (y - curx - yShift),
    LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x + curx + xShift), (y + cury + yShift),
    LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - curx - xShift), (y + cury + yShift),
    LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x + cury + xShift), (y + curx + yShift),
    LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - cury - xShift), (y + curx + yShift),
    LCD_COLOR_BLACK);

    if (d < 0) {
      d += (curx << 2) + 6;
    } else {
      d += ((curx - cury) << 2) + 10;
      cury--;
    }
    curx++;
  }

  BSP_LCD_DrawHLine(x - xShift, y - CARD_HALFHEIGHT, xShift * 2);
  BSP_LCD_DrawHLine(x - xShift, y + CARD_HALFHEIGHT, xShift * 2);
  BSP_LCD_DrawVLine(x - CARD_HALFWIDTH, y - yShift, yShift * 2);
  BSP_LCD_DrawVLine(x + CARD_HALFWIDTH, y - yShift, yShift * 2);
}

/* Draw a card centered at (x,y) */
void drawCard(uint16_t x, uint16_t y, int value, bool selected) {
  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_FillRect(x - CARD_HALFWIDTH - CARD_MARGIN,
      y - CARD_HALFHEIGHT - CARD_MARGIN, CARD_WIDTH + 2 * CARD_MARGIN,
      CARD_HEIGHT + 2 * CARD_MARGIN);

// If card is highlighted, add gray background
  if (selected) {
    BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
    BSP_LCD_FillCircle(x - CARD_HALFWIDTH, y - CARD_HALFHEIGHT, CARD_HIGHLIGHT);
    BSP_LCD_FillCircle(x + CARD_HALFWIDTH - 1, y - CARD_HALFHEIGHT,
    CARD_HIGHLIGHT);
    BSP_LCD_FillCircle(x - CARD_HALFWIDTH, y + CARD_HALFHEIGHT - 1,
    CARD_HIGHLIGHT);
    BSP_LCD_FillCircle(x + CARD_HALFWIDTH - 1, y + CARD_HALFHEIGHT - 1,
    CARD_HIGHLIGHT);
    BSP_LCD_FillRect(x - CARD_HALFWIDTH, y - CARD_HALFHEIGHT - CARD_HIGHLIGHT,
    CARD_WIDTH, CARD_HEIGHT + 2 * CARD_HIGHLIGHT);
    BSP_LCD_FillRect(x - CARD_HALFWIDTH - CARD_HIGHLIGHT, y - CARD_HALFHEIGHT,
    CARD_WIDTH + 2 * CARD_HIGHLIGHT, CARD_HEIGHT);
  }
  drawRoundedCard(x, y);

  // If card value has a 1 bit in the 1's place, draw red dot
  if (value & 0x01) {
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_FillCircle(x - CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);
  }

  // If card value has a 1 bit in the 2's place, draw orange dot
  if (value & 0x02) {
    BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
    BSP_LCD_FillCircle(x + CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);
  }

  // If card value has a 1 bit in the 4's place, draw yellow dot
  if (value & 0x04) {
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_FillCircle(x - CARD_HALFUNIT, y, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y, DOT_RADIUS);
  }

  // If card value has a 1 bit in the 8's place, draw dark green dot
  if (value & 0x08) {
    BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
    BSP_LCD_FillCircle(x + CARD_HALFUNIT, y, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y, DOT_RADIUS);
  }

  // If card value has a 1 bit in the 16's place, draw blue dot
  if (value & 0x10) {
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_FillCircle(x - CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);
  }

  // If card value has a 1 bit in the 32's place, draw magenta dot
  if (value & 0x20) {
    BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
    BSP_LCD_FillCircle(x + CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);
  }

  // If card value has a 1 bit in the 64's place, draw cyan dot
  if (value & 0x40) {
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_FillCircle(x, y - CARD_HALFUNIT, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x, y - CARD_HALFUNIT, DOT_RADIUS);
  }

  // If card value has a 1 bit in the 128's place, draw dark gray dot
  if (value & 0x80) {
    BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
    BSP_LCD_FillCircle(x, y - CARD_HALFUNIT, DOT_RADIUS);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x, y - CARD_HALFUNIT, DOT_RADIUS);
  }
}

void clearScreen() {
  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_FillRect(0, 0, 240, 300);
}

void prepareDisplay(void) {
  BSP_LCD_DisplayOn();
  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_SetFont(&Font16Condensed);
}

// Take time (in milliseconds) and print it on the LCD
void drawGameTime(uint32_t time, bool blue) {
  // Clear the existing time display
  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_FillRect(0, 300, 85, 20);

  uint16_t s = time / 1000;
  uint16_t h = s / 3600;
  s -= h * 3600;
  uint16_t m = s / 60;
  s -= m * 60;
  char string[9]; // Time is of form HH:MM:SS, rolls over after a bit over 18 hours
  sprintf(string, "%02d:%02d:%02d", h, m, s);
  BSP_LCD_SetTextColor(blue ? LCD_COLOR_BLUE : LCD_COLOR_BLACK);
  BSP_LCD_DisplayStringAt(8, 300, (uint8_t*) string, LEFT_MODE);
}

// Take remaining deck size and print it on the LCD
void drawGameCardCount(uint16_t cardsLeft) {
  // Clear the existing sets display
  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_FillRect(85, 300, 155, 20);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  char string[16]; // Assume number of cards never exceeds 3 digits (reasonable when the upper limit is 256 cards)
  sprintf(string, "Cards left: %d", cardsLeft);
  BSP_LCD_DisplayStringAt(-3, 300, (uint8_t*) string, RIGHT_MODE);
}


void drawGameWon(int level) {
  char winMsg[20];
  sprintf(winMsg, "Level %d", level);

  // Clear the existing sets display
  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
  BSP_LCD_FillRect(85, 300, 155, 20);
  BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);

  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAt(-3, 280, (uint8_t*)"You won!", RIGHT_MODE);
  BSP_LCD_DisplayStringAt(-11, 300, (uint8_t*) winMsg, RIGHT_MODE);
  BSP_LCD_SetFont(&Font16Condensed);

}
