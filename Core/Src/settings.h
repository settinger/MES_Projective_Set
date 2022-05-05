/*
 * settings.h
 * Originally this was going to define a struct to hold user
 * preferences and settings in the emulated EEPROM. However,
 * the EEPROM can only store 16-bit values, so some creative
 * adjustments had to be made
 *
 * TODO: edit linker to protect EEPROM region from being overwritten by code
 *
 *  Created on: Apr 22, 2022
 *      Author: Sam
 */
#include "stdio.h"

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

/* Define the address of the first 16-bit value of user preferences/settings.
 * If the start address is n, then the values stored are:
 * n:   EEPROM check 0     (uint16_t)
 * n+1: EEPROM check 1     (uint16_t)
 * n+2: EEPROM check 2     (uint16_t)
 * n+3: EEPROM check 3     (uint16_t)
 * n+4: LCD calibration A1 (int)
 * n+5: LCD calibration A2 (int)
 * n+6: LCD calibration B1 (int)
 * n+7: LCD calibration B2 (int)
 * n+8: Proset Game Level  (uint16_t)
 */
#define SETTINGS_START_ADDRESS 0x5555

// Check for these four values in the initial four EEPROM bytes
// This ensures that we only ever try to read  valid EEPROM data
// However there's no guarantee that we aren't instead writing
//   over important code; should check for FF FF FF FF I think
//#define EEPROM_CHECK 0x24596B84EA78324DULL
#define EEPROM_CHECK_0 0x2459
#define EEPROM_CHECK_1 0xB84E
#define EEPROM_CHECK_2 0xA783
#define EEPROM_CHECK_3 0x8324
#define EEPROM_CHECK_0_ADDRESS SETTINGS_START_ADDRESS
#define EEPROM_CHECK_1_ADDRESS SETTINGS_START_ADDRESS+1
#define EEPROM_CHECK_2_ADDRESS SETTINGS_START_ADDRESS+2
#define EEPROM_CHECK_3_ADDRESS SETTINGS_START_ADDRESS+3

#define EEPROM_LCD_A1_ADDRESS  SETTINGS_START_ADDRESS+4
#define EEPROM_LCD_A2_ADDRESS  SETTINGS_START_ADDRESS+5
#define EEPROM_LCD_B1_ADDRESS  SETTINGS_START_ADDRESS+6
#define EEPROM_LCD_B2_ADDRESS  SETTINGS_START_ADDRESS+7

#define EEPROM_PROSET_ADDRESS  SETTINGS_START_ADDRESS+8

int EEPROM_exists(void);

#endif /* INC_SETTINGS_H_ */
