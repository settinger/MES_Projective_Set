/*
 * game.c
 *
 *  Created on: May 4, 2022
 *      Author: Sam
 */

#include "game.h"
#include "eeprom.h"
#include "settings.h"
#include "serial_logging.h"


/*
 * Load difficulty level from EEPROM
 * If found, set up game for that difficulty level
 * If not found, use Difficulty 6
 * TODO: Possibly consider combining with the touchscreen.c EEPROM read event
 */
static uint16_t eepromGetLevel(void) {
  uint16_t level = 6;
  Serial_Message("EEPROM initializing for level select...");
  HAL_FLASH_Unlock();
  HAL_Delay(10);
  if (EE_OK == EE_Init()) {
    Serial_Message("EEPROM initialized for level select.");
    uint16_t readInt;
    if ((EE_ReadVariable(EEPROM_PROSET_ADDRESS, &readInt)) == HAL_OK) {
      Serial_Message("Level loaded!");
      level = readInt;
    }
  }
  HAL_FLASH_Lock();
  return level;
}

/*
 * Game init
 * Look for difficulty level in EEPROM
 * Depending on level, lay out game board in a particular way
 * Initialize deck, shuffle deck
 */
void prosetInit(void) {
  uint16_t level = eepromGetLevel();
}
