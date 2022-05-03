/*
 * display.h
 *
 *  Created on: Apr 23, 2022
 *      Author: Sam
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#define LCD_FRAME_BUFFER_LAYER0 (LCD_FRAME_BUFFER + 0x130000)
#define LCD_FRAME_BUFFER_LAYER1 LCD_FRAME_BUFFER

void prepareDisplay(void);

#endif /* SRC_DISPLAY_H_ */
