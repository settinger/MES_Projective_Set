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

// Structure of game input response
typedef enum gameStatus {
  GAME_IN_PLAY = 0U,
  GAME_ENDED = 2U,
  GAME_RESET = 3U,
  GAME_LEVEL_SELECT = 4U,
  GAME_ERROR = 0XFFU
} gameStatus;

void prosetInit(void);
void drawTable(void);
void drawTime(uint32_t time, bool gameComplete);
void drawCardCount();
void drawGameWon();
gameStatus gameTouchHandler(uint16_t x, uint16_t y);
gameStatus gameProcessInput(char oneChar);

#endif /* SRC_PROSET_GAME_H_ */
