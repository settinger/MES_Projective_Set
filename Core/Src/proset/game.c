/*
 * game.c
 *
 *  Created on: May 4, 2022
 *      Author: Sam
 */

#include "game.h"
#include "card.h"
#include "deck.h"
#include "eeprom.h"
#include "settings.h"
#include "serial_logging.h"
#include "display.h"

uint16_t numDots;
uint16_t deckSize;
uint16_t cardsOnTable;
Card deck[MAX_CARDS];
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
    uint16_t readInt;
    if ((EE_ReadVariable(EEPROM_PROSET_ADDRESS, &readInt)) == HAL_OK) {
      Serial_Message("Level loaded!");
      Print_Int(readInt);
      numDots = readInt;
    }
  }
  HAL_FLASH_Lock();
  deckSize = (1 << numDots) - 1;
  cardsOnTable = numDots + 1;
}

/*
 * Send debug instruction to show table/deck status
 */
static void gameStatus(void) {
  Serial_Message_NB("Number of dots: ");
  Print_Int(numDots);
  Serial_Message_NB("Deck: ");
  for (int i=0; i<deckSize; i++) {
    Print_Int_NB(deck[i].value);
    Serial_Message_NB(", ");
  }
}

static void initTable(void) {
  // Assign the CardSlot structs

  for (uint16_t i; i < cardsOnTable; i++) {
    table[i].holdsCard = false;
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
 * Game init
 * Look for difficulty level in EEPROM
 * Depending on level, lay out game board in a particular way
 * Initialize deck, shuffle deck
 */
void prosetInit(void) {
  eepromGetLevel();
  initDeck();
  initTable();
  // TODO: Begin timer
  //deal();
  gameStatus();
}
