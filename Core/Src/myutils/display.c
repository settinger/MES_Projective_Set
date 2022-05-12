/*
 * display.c
 *
 *  Created on: Apr 23, 2022
 *      Author: Sam
 */

#include "display.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "stdio.h"

/*
 * Board dimensions
 * For now, assume 240x320 pixel display, origin in top left
 */

/* Draw a card centered at (x,y) */
void drawRoundedCard(uint16_t x, uint16_t y) {
  uint16_t xShift = CARD_HALFWIDTH-DOT_RADIUS;
  uint16_t yShift = CARD_HALFHEIGHT-DOT_RADIUS;

  /* Draw the background fill */
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillCircle(x - xShift, y - yShift, DOT_RADIUS);
  BSP_LCD_FillCircle(x + xShift, y - yShift, DOT_RADIUS);
  BSP_LCD_FillCircle(x - xShift, y + yShift, DOT_RADIUS);
  BSP_LCD_FillCircle(x + xShift, y + yShift, DOT_RADIUS);
  BSP_LCD_FillRect(x-xShift, y-CARD_HALFHEIGHT, CARD_WIDTH-DOT_RADIUS*2, CARD_HEIGHT);
  BSP_LCD_FillRect(x-CARD_HALFWIDTH, y-yShift, CARD_WIDTH, CARD_HEIGHT-DOT_RADIUS*2);
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
    BSP_LCD_DrawPixel((x + curx + xShift), (y - cury - yShift), LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - curx - xShift), (y - cury - yShift), LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x + cury + xShift), (y - curx - yShift), LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - cury - xShift), (y - curx - yShift), LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x + curx + xShift), (y + cury + yShift), LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - curx - xShift), (y + cury + yShift), LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x + cury + xShift), (y + curx + yShift), LCD_COLOR_BLACK);
    BSP_LCD_DrawPixel((x - cury - xShift), (y + curx + yShift), LCD_COLOR_BLACK);

    if (d < 0) {
      d += (curx << 2) + 6;
    } else {
      d += ((curx - cury) << 2) + 10;
      cury--;
    }
    curx++;
  }

  BSP_LCD_DrawHLine(x-xShift, y-CARD_HALFHEIGHT, xShift*2);
  BSP_LCD_DrawHLine(x-xShift, y+CARD_HALFHEIGHT, xShift*2);
  BSP_LCD_DrawVLine(x-CARD_HALFWIDTH, y-yShift, yShift*2);
  BSP_LCD_DrawVLine(x+CARD_HALFWIDTH, y-yShift, yShift*2);
}

/* Draw a card centered at (x,y) */
void drawCard(uint16_t x, uint16_t y) {
//  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
//  BSP_LCD_FillRect(x - CARD_HALFWIDTH, y - CARD_HALFHEIGHT, CARD_WIDTH,
//  CARD_HEIGHT);
//  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  //BSP_LCD_DrawRect(x - CARD_HALFWIDTH, y - CARD_HALFHEIGHT, CARD_WIDTH, CARD_HEIGHT);
  drawRoundedCard(x, y);

  // For now, just draw all dots
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_FillCircle(x - CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
  BSP_LCD_FillCircle(x + CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y - CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_FillCircle(x - CARD_HALFUNIT, y, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
  BSP_LCD_FillCircle(x + CARD_HALFUNIT, y, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillCircle(x - CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
  BSP_LCD_FillCircle(x + CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y + CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  BSP_LCD_FillCircle(x, y - CARD_HALFUNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x, y - CARD_HALFUNIT, DOT_RADIUS);
}

void prepareDisplay(void) {
  BSP_LCD_Init();
  BSP_LCD_DisplayOn();

  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);

  drawCard(BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH,
  CARD_MARGIN + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH,
  CARD_MARGIN + CARD_HALFHEIGHT);

  drawCard(BOARD_HALFWIDTH - CARD_MARGIN * 2 - CARD_WIDTH,
  CARD_MARGIN * 3 + CARD_HEIGHT + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH, CARD_MARGIN * 3 + CARD_HEIGHT + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH + CARD_MARGIN * 2 + CARD_WIDTH,
  CARD_MARGIN * 3 + CARD_HEIGHT + CARD_HALFHEIGHT);

  drawCard(BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH,
  CARD_MARGIN * 5 + CARD_HEIGHT * 2 + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH,
  CARD_MARGIN * 5 + CARD_HEIGHT * 2 + CARD_HALFHEIGHT);

  BSP_LCD_DisplayStringAt(0, 300, (uint8_t*)"04:20", RIGHT_MODE);
  BSP_LCD_DisplayStringAt(0, 300, (uint8_t*)"0 Sets", LEFT_MODE);
}
