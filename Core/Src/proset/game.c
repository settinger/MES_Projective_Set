/*
 * game.c
 *
 *  Created on: May 4, 2022
 *      Author: Sam
 */

#include "game.h"
#include <cards.h>
#include <game_graphics.h>
#include "eeprom.h"
#include "settings.h"
#include "serial_logging.h"

/* Globals for difficulty level, deck stuff */
uint16_t numDots;
uint16_t deckSize;
uint16_t tableCards;
int deck[MAX_CARDS];
int deckPointer = -1; // Index of next card to be dealt from the deck array
CardSlot table[10];

/*
 * Load difficulty level from EEPROM
 * If found, set up game for that difficulty level
 * If not found, use Difficulty 6
 * TODO: Possibly consider combining with the touchscreen.c EEPROM read event
 * TODO: Write a value here for the first time
 * TODO: Set linker to avoid writing code into EEPROM space
 */
static void eepromGetLevel(void) {
  numDots = 6;
  Serial_Message("EEPROM initializing for level select...");
  HAL_FLASH_Unlock();
  HAL_Delay(10);
  if (EE_OK == EE_Init()) {
    Serial_Message("EEPROM initialized for level select.");

    /*
     if ((EE_WriteVariable(EEPROM_PROSET_ADDRESS, 7)) != HAL_OK) {
     Serial_Message("Error writing to EEPROM: Proset Address");
     return;
     } else {
     Serial_Message("Level written, nice");
     }
     */

    uint16_t readInt;
    if ((EE_ReadVariable(EEPROM_PROSET_ADDRESS, &readInt)) == HAL_OK) {
      Serial_Message("Level loaded!");
      Print_Int(readInt);
      numDots = readInt;
    } else {
      Serial_Message("No level loaded; defaulting to 6");
    }
  }
  HAL_FLASH_Lock();
  deckSize = (1 << numDots) - 1;
  tableCards = numDots + 1;
}

/*
 * Send debug instruction to show table/deck status
 */
static void printGameStatus(void) {
  Serial_Message_NB("Number of dots: ");
  Print_Int(numDots);
  Serial_Message_NB("Cards left in deck: ");
  for (int i = deckPointer; i < deckSize; i++) {
    Print_Int_NB(deck[i]);
    Serial_Message_NB(", ");
  }
  char board[100];
  switch (numDots) {
  case 4:
    sprintf(board, " %02d  %02d\r\n   %02d\r\n %02d  %02d\r\n",
        table[0].cardVal, table[1].cardVal, table[2].cardVal, table[3].cardVal,
        table[4].cardVal);
    break;
  case 5:
    sprintf(board, " %02d  %02d\r\n %02d  %02d\r\n %02d  %02d\r\n",
        table[0].cardVal, table[1].cardVal, table[2].cardVal, table[3].cardVal,
        table[4].cardVal, table[5].cardVal);
    break;
  case 6:
    sprintf(board, "   %02d  %02d\r\n %02d  %02d  %02d\r\n   %02d  %02d\r\n",
        table[0].cardVal, table[1].cardVal, table[2].cardVal, table[3].cardVal,
        table[4].cardVal, table[5].cardVal, table[6].cardVal);
    break;
  case 7:
    sprintf(board,
        " %03d  %03d  %03d\r\n    %03d  %03d\r\n %03d  %03d  %03d\r\n",
        table[0].cardVal, table[1].cardVal, table[2].cardVal, table[3].cardVal,
        table[4].cardVal, table[5].cardVal, table[6].cardVal, table[7].cardVal);
    break;
  case 8:
    sprintf(board,
        " %03d  %03d  %03d\r\n %03d  %03d  %03d\r\n %03d  %03d  %03d\r\n",
        table[0].cardVal, table[1].cardVal, table[2].cardVal, table[3].cardVal,
        table[4].cardVal, table[5].cardVal, table[6].cardVal, table[7].cardVal,
        table[8].cardVal);
    break;
  }
  Serial_Message("\r\nTable:");
  Serial_Message(board);
}

static void initTable(void) {
  // Assign the CardSlot structs

  for (uint16_t i = 0; i < tableCards; i++) {
    table[i].cardVal = -1;
    table[i].selected = false;
  }
  switch (numDots) {
  case 4: // 5 cards in 3 rows: 2, 1, 2
    table[0].x = COLUMN_2;
    table[0].y = ROW_1;
    table[1].x = COLUMN_4;
    table[1].y = ROW_1;
    table[2].x = COLUMN_3;
    table[2].y = ROW_2;
    table[3].x = COLUMN_2;
    table[3].y = ROW_3;
    table[4].x = COLUMN_4;
    table[4].y = ROW_3;
    break;
  case 5: // 6 cards in 3 rows: 2, 2, 2
    table[0].x = COLUMN_2;
    table[0].y = ROW_1;
    table[1].x = COLUMN_4;
    table[1].y = ROW_1;
    table[2].x = COLUMN_2;
    table[2].y = ROW_2;
    table[3].x = COLUMN_4;
    table[3].y = ROW_2;
    table[4].x = COLUMN_2;
    table[4].y = ROW_3;
    table[5].x = COLUMN_4;
    table[5].y = ROW_3;
    break;
  case 6: // 7 cards in 3 rows: 2, 3, 2
    table[0].x = COLUMN_2;
    table[0].y = ROW_1;
    table[1].x = COLUMN_4;
    table[1].y = ROW_1;
    table[2].x = COLUMN_1;
    table[2].y = ROW_2;
    table[3].x = COLUMN_3;
    table[3].y = ROW_2;
    table[4].x = COLUMN_5;
    table[4].y = ROW_2;
    table[5].x = COLUMN_2;
    table[5].y = ROW_3;
    table[6].x = COLUMN_4;
    table[6].y = ROW_3;
    break;
  case 7: // 8 cards in 3 rows: 3, 2, 3
    table[0].x = COLUMN_1;
    table[0].y = ROW_1;
    table[1].x = COLUMN_3;
    table[1].y = ROW_1;
    table[2].x = COLUMN_5;
    table[2].y = ROW_1;
    table[3].x = COLUMN_2;
    table[3].y = ROW_2;
    table[4].x = COLUMN_4;
    table[4].y = ROW_2;
    table[5].x = COLUMN_1;
    table[5].y = ROW_3;
    table[6].x = COLUMN_3;
    table[6].y = ROW_3;
    table[7].x = COLUMN_5;
    table[7].y = ROW_3;
    break;
  case 8: // 9 cards in 3 rows: 3, 3, 3
    table[0].x = COLUMN_1;
    table[0].y = ROW_1;
    table[1].x = COLUMN_3;
    table[1].y = ROW_1;
    table[2].x = COLUMN_5;
    table[2].y = ROW_1;
    table[3].x = COLUMN_1;
    table[3].y = ROW_2;
    table[4].x = COLUMN_3;
    table[4].y = ROW_2;
    table[5].x = COLUMN_5;
    table[5].y = ROW_2;
    table[6].x = COLUMN_1;
    table[6].y = ROW_3;
    table[7].x = COLUMN_3;
    table[7].y = ROW_3;
    table[8].x = COLUMN_5;
    table[8].y = ROW_3;
    break;
    // Not enough space on screen for 9dot version :(
  }
}

/*
 * While there are empty slots (and while deck is not exhausted), deal cards
 * into empty cardSlot structs.
 */
static void dealCards(void) {
  for (int i = 0; i < tableCards; i++) {
    if ((table[i].cardVal < 0) && (deckPointer < deckSize)) {
      table[i].cardVal = deck[deckPointer];
      deckPointer++;
    }
  }
  // Update the "cards remaining" counter
  drawCardCount();
}

// Draw the cards currently on the table
void drawTable() {
  clearScreen();
  for (int i = 0; i < tableCards; i++) {
    if (table[i].cardVal > 0) {
      drawCard(table[i].x, table[i].y, table[i].cardVal, table[i].selected);
    }
  }
}

/*
 * Game init
 * Look for difficulty level in EEPROM
 * Depending on level, lay out game board in a particular way
 * Initialize deck, shuffle deck
 */
void prosetInit(void) {
  eepromGetLevel();
  initDeck();
  deckPointer = 0;
  initTable();
  prepareDisplay();
  dealCards();
  drawTable();
  drawTime(0, false);
#ifdef DEBUG
  printGameStatus();
#endif
}

/*
 * Draw the clock
 * Interface with game_graphics library
 */
void drawTime(uint32_t time, bool gameComplete) {
  drawGameTime(time, gameComplete);
}

/*
 * Draw the sets counter text
 * Interface with game_graphics library
 */
void drawCardCount() {
  drawGameCardCount(deckSize - deckPointer);
}

/*
 * Check if the currently selected set is valid
 */
static bool selectionIsValid(void) {
  uint8_t sizeOfSet = 0;
  uint8_t xorOfSet = 0;
  for (int i = 0; i < tableCards; i++) {
    if ((table[i].cardVal > 0) && table[i].selected) {
      sizeOfSet++;
      xorOfSet ^= table[i].cardVal;
    }
  }
  return ((sizeOfSet >= 3) && (xorOfSet == 0));
}

/*
 * Check if the win conditions have been met (no cards on table, no cards in deck)
 */
static bool gameComplete(void) {
  if (deckPointer < deckSize) {
    return false;
  }
  for (int i = 0; i < tableCards; i++) {
    if (table[i].cardVal > 0) {
      return false;
    }
  }
  return true;
}

/*
 * Take the currently selected set, remove, and replace
 */
static void takeAwaySet(void) {
#ifdef DEBUG
  Serial_Message("\r\nSet was found!");
#endif
  // Remove all selected cards, set slot cardval to -1
  // If deck and table exhausted, enter win state
  // Deal new cards until deck is exhausted
  for (int i = 0; i < tableCards; i++) {
    if (table[i].selected) {
      table[i].selected = false;
      table[i].cardVal = -1;
    }
  }
  dealCards();
}

/*
 * Update the game based on a touch occurring at (x, y)
 * Interfaces with game_graphics library
 * Returns GAME_IN_PLAY or GAME_ENDED if game is completed by this action
 */
gameStatus gameTouchHandler(uint16_t x, uint16_t y) {
  for (int i = 0; i < tableCards; i++) {
    if ((table[i].cardVal > 0) && CARDHIT(table[i].x, table[i].y, x, y)) {
      table[i].selected = !table[i].selected;
      // Do validity-of-set calculations here
      if (selectionIsValid()) {
        takeAwaySet();
        drawTable();
        if (gameComplete()) {
          // TODO: do win conditions here
          // Timer gets stopped in main.c by returning true, so don't worry about that
          // Change "Cards left: 0" text to "Game complete!" or "You win!" or something
          // Set timer text to blue
          char winMsg[20];
          sprintf(winMsg, "You won lvl %d!", numDots);
          Serial_Message(winMsg);
          drawGameWon(numDots);
#ifdef DEBUG
          Serial_Message("Game complete!");
#endif
          return GAME_ENDED;
        }
      } else {
        drawCard(table[i].x, table[i].y, table[i].cardVal, table[i].selected);
      }
      break;
    }
  }
  return GAME_IN_PLAY;
}

/*
 * Update the game based on a keystroke received from console.
 * Returns "true" if game is completed by this action.
 * It would be better to do this with a consoleCommandTable struct,
 *   like we did in the console assignment, not a big if-else construction.
 * Interfaces with game_graphics library
 * Valid inputs are:
 *     Numerals 1-9         toggle cards on table
 *     'c'                  deselects all cards or, if no
 *                            cards selected, selects all cards
 *     'r'                  starts a new game
 *     'l'                  opens level select screen
 */
gameStatus gameProcessInput(char oneChar) {
  if ((48 < oneChar) && (oneChar <= (48 + tableCards))) {
    // Act like a card was touched
    int cardIndex = oneChar-49; // e.g. ASCII 49 "1" corresponds to card 0
    return gameTouchHandler(table[cardIndex].x, table[cardIndex].y);
  } else if ((oneChar == 'C') || (oneChar == 'c')) {
    // Check if any cards are selected currently
    bool anySelected = false;
    for (int i = 0; i < tableCards; i++) {
      if (table[i].cardVal > 0 && table[i].selected) {
        anySelected = true;
        break;
      }
    }
    // If any cards currently selected, deselect all; else, select all and check if valid set
    for (int i = 0; i < tableCards; i++) {
      if (table[i].cardVal > 0) {
        table[i].selected = !anySelected;
      }
    }
    if (!anySelected && selectionIsValid()) {
      takeAwaySet();
      drawTable();
      if (gameComplete()) {
        // TODO: do win conditions here
        drawGameWon(numDots);
#ifdef DEBUG
        Serial_Message("Game complete!");
#endif
        return GAME_ENDED;
      }
    } else {
      drawTable();
    }
  } else if ((oneChar == 'R') || (oneChar == 'r')) {
    //    lastFrameTick = HAL_GetTick();
    //    lastSecondTick = lastFrameTick;
    //    gameStart = lastSecondTick + 250; // Hacky way to add a grace period before clock starts
    //    prosetInit();
    return GAME_RESET;
  } else if ((oneChar == 'L') || (oneChar == 'l')) {
    ; // Did I implement level select yet?
  }
  return GAME_IN_PLAY;
}
