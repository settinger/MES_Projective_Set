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
  SYSTEM_BOOT = 0U,
  GAME_INIT = 1U,
  GAME_IN_PLAY = 2U,
  GAME_WIN = 3U,
  GAME_ENDED = 4U,
  GAME_ENTER_LEVEL_SELECT = 5U,
  GAME_LEVEL_SELECT = 6U,
  GAME_ERROR = 0XFFU
} gameStatus;

void eepromGetLevel(void);
void prosetInit(void);
void levelSelectInit(void);
void drawTable(void);
void drawTime(uint32_t time, bool gameComplete);
void drawCardCount();
void drawGameWon();
gameStatus gameTouchHandler(uint16_t x, uint16_t y);
gameStatus gameProcessInput(char oneChar);
gameStatus levelSelectTouchHandler(uint16_t x, uint16_t y);
gameStatus levelSelectProcessInput(char oneChar);
gameStatus clearTable(void);

#endif /* SRC_PROSET_GAME_H_ */
