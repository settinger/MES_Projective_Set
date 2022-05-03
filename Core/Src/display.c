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

void prepareDisplay(void) {
  BSP_LCD_Init();
  // Set up Layer 1 for white
  BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER_LAYER1);
  BSP_LCD_SelectLayer(1);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetColorKeying(1, LCD_COLOR_WHITE);
  BSP_LCD_SetLayerVisible(1, DISABLE);

  // Set up Layer 0
  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);
  BSP_LCD_SelectLayer(0);

  BSP_LCD_DisplayOn();
  BSP_LCD_Clear(LCD_COLOR_BLUE);
  BSP_LCD_DisplayStringAtLine(2, (uint8_t*) " Thank you ");
  BSP_LCD_DisplayStringAtLine(3, (uint8_t*) "  XXXXXX!  ");
}
