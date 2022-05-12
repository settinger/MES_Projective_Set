/*
 * deck.h
 *
 *  Created on: May 6, 2022
 *      Author: Sam
 */

#ifndef SRC_PROSET_CARDS_H_
#define SRC_PROSET_CARDS_H_

#include "stdio.h"
#include <stdbool.h>

#define MAX_CARDS 512 // A deck at level 9 would have 2^9-1 (511) cards in it

/*
typedef struct Card{
  uint16_t value;
} Card;
*/

typedef struct CardSlot{
  uint16_t x;
  uint16_t y;
  bool selected;
  int cardVal;
} CardSlot;

void shuffle(void);
void initDeck(void);
void dealCard(CardSlot*);

#endif /* SRC_PROSET_CARDS_H_ */
