/*
 * game.h
 *
 *  Created on: May 4, 2022
 *      Author: Sam
 */

#ifndef SRC_PROSET_GAME_H_
#define SRC_PROSET_GAME_H_

#include <stdio.h>
#include <stdbool.h>

void prosetInit(void);
void drawTable(void);
void drawTime(uint32_t time, bool gameComplete);
void drawCardCount();
void drawGameWon();
bool gameTouchHandler(uint16_t x, uint16_t y);
bool gameProcessInput(char oneChar);

#endif /* SRC_PROSET_GAME_H_ */
