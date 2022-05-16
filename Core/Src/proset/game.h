/*
 * game.h
 *
 *  Created on: May 4, 2022
 *      Author: Sam
 */

#ifndef SRC_PROSET_GAME_H_
#define SRC_PROSET_GAME_H_

#include <stdio.h>

void prosetInit(void);
void drawTable(void);
void drawTime(uint32_t time);
void drawSets(uint16_t sets);
void gameTouchHandler(uint16_t x, uint16_t y);

#endif /* SRC_PROSET_GAME_H_ */
