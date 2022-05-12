/*
 * settings.c
 *
 *  Created on: Apr 22, 2022
 *      Author: Sam
 */

#include "settings.h"
#include "eeprom.h"

/* EEPROM private variables */
/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NB_OF_VAR] = { SETTINGS_START_ADDRESS };
uint16_t VarDataTab[NB_OF_VAR] = { 0 };
uint16_t VarValue, VarDataTmp = 0;

int EEPROM_exists(void) {
  return 1;
}
