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
 * Cards should be 25:32 aspect with 6-7 units between them
 * padding of 2 units for card margin, so 21:28 inside
 * So...try 1 unit=2.5px?
 *
 * Card: 60x84 px, 5:7 aspect
 * Interior padding: 6px, so card interior is 48x72, 2:3 aspect
 * Dots are: 16px diameter (can be tweaked)
 */

/* Draw a card centered at (x,y) */
void drawCard(uint16_t x, uint16_t y) {
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillRect(x-CARD_HALFWIDTH, y-CARD_HALFHEIGHT, CARD_WIDTH, CARD_HEIGHT);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawRect(x-CARD_HALFWIDTH, y-CARD_HALFHEIGHT, CARD_WIDTH, CARD_HEIGHT);

  // For now, just draw all dots
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_FillCircle(x - CARD_HALFUNIT, y-CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y-CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
  BSP_LCD_FillCircle(x + CARD_HALFUNIT, y-CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y-CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_FillCircle(x - CARD_HALFUNIT, y, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
  BSP_LCD_FillCircle(x + CARD_HALFUNIT, y, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillCircle(x - CARD_HALFUNIT, y+CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_DrawCircle(x - CARD_HALFUNIT, y+CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
  BSP_LCD_FillCircle(x + CARD_HALFUNIT, y+CARD_UNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x + CARD_HALFUNIT, y+CARD_UNIT, DOT_RADIUS);

  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  BSP_LCD_FillCircle(x, y-CARD_HALFUNIT, DOT_RADIUS);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawCircle(x, y-CARD_HALFUNIT, DOT_RADIUS);
}

void prepareDisplay(void) {
  BSP_LCD_Init();
  BSP_LCD_DisplayOn();

  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);

  drawCard(BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH, CARD_MARGIN + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH, CARD_MARGIN + CARD_HALFHEIGHT);

  drawCard(BOARD_HALFWIDTH - CARD_MARGIN*2 - CARD_WIDTH, CARD_MARGIN*3 + CARD_HEIGHT + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH, CARD_MARGIN*3 + CARD_HEIGHT + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH + CARD_MARGIN*2 + CARD_WIDTH, CARD_MARGIN*3 + CARD_HEIGHT + CARD_HALFHEIGHT);

  drawCard(BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH, CARD_MARGIN*5 + CARD_HEIGHT*2 + CARD_HALFHEIGHT);
  drawCard(BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH, CARD_MARGIN*5 + CARD_HEIGHT*2 + CARD_HALFHEIGHT);

//  BSP_LCD_DisplayStringAtLine(2, (uint8_t*) " Thank you ");
//  BSP_LCD_DisplayStringAtLine(3, (uint8_t*) "  XXXXXX!  ");
}
