/*
 * card.h
 * Defines the Card object
 *
 *  Created on: May 4, 2022
 *      Author: Sam
 */

#ifndef SRC_PROSET_CARD_H_
#define SRC_PROSET_CARD_H_

#include "stdio.h"
#include <stdbool.h>

typedef struct Card{
  uint16_t value;
} Card;

typedef struct CardSlot{
  uint16_t x;
  uint16_t y;
  bool selected;
  bool holdsCard; // Might be unneccesary
  Card heldCard;
} CardSlot;

#endif /* SRC_PROSET_CARD_H_ */
