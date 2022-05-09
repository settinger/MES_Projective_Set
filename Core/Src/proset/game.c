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
      numDots = readInt;
    }
  }
  HAL_FLASH_Lock();
  deckSize = (1 << numDots) - 1;
  cardsOnTable = numDots + 1;
}

/*
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
  */

static void initTable(void) {
  // Assign the CardSlot structs
  // numDots == 4: 5 cards in 3 rows: 2, 1, 2
  // numDots == 5: 6 cards in 3 rows: 2, 2, 2
  // numDots == 6: 7 cards in 3 rows: 2, 3, 2
  // numDots == 7: 8 cards in 3 rows: 3, 2, 3
  // numDots == 8: 9 cards in 3 rows: 3, 3, 3
  // Not enough space on screen for 9dot version :(
  for (uint16_t i; i < cardsOnTable; i++) {
    table[i].holdsCard = false;
  }
  switch (numDots) {
  case 4:
    table[0].x = BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH;
    table[0].y = CARD_MARGIN + CARD_HALFHEIGHT;
    table[1].x = BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH;
    table[1].y = CARD_MARGIN + CARD_HALFHEIGHT;
    table[2].x = BOARD_HALFWIDTH;
    table[2].y = 3*CARD_MARGIN + CARD_HEIGHT + CARD_HALFHEIGHT;
    table[3].x = BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH;
    table[3].y = 5*CARD_MARGIN + 2*CARD_HEIGHT + CARD_HALFHEIGHT;
    table[4].x = BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH;
    table[4].y = 5*CARD_MARGIN + 2*CARD_HEIGHT + CARD_HALFHEIGHT;
    break;
  case 5:
    break;
  case 6:
    table[0].x = BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH;
    table[0].y = CARD_MARGIN + CARD_HALFHEIGHT;
    table[1].x = BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH;
    table[1].y = CARD_MARGIN + CARD_HALFHEIGHT;
    table[2].x = BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH;
    table[2].y = 3*CARD_MARGIN + CARD_HEIGHT + CARD_HALFHEIGHT;
    table[3].x = BOARD_HALFWIDTH;
    table[3].y = 3*CARD_MARGIN + CARD_HEIGHT + CARD_HALFHEIGHT;
    //
    table[4].y = 3*CARD_MARGIN + CARD_HEIGHT + CARD_HALFHEIGHT;
    table[5].x = BOARD_HALFWIDTH - CARD_MARGIN - CARD_HALFWIDTH;
    table[5].y = 5*CARD_MARGIN + 2*CARD_HEIGHT + CARD_HALFHEIGHT;
    table[6].x = BOARD_HALFWIDTH + CARD_MARGIN + CARD_HALFWIDTH;
    table[6].y = 5*CARD_MARGIN + 2*CARD_HEIGHT + CARD_HALFHEIGHT;
    break;
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
}
