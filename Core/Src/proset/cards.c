/*
 * deck.c
 *
 *  Created on: May 6, 2022
 *      Author: Sam
 */

#include <cards.h>
#include <stdlib.h>
#include "stdio.h"
#include "serial_logging.h"
#include "rng.h"

/* Globals for difficulty level, deck stuff */

extern uint16_t numDots;
extern uint16_t deckSize;
extern int deck[MAX_CARDS];
extern int deckPointer;

/*
 * Fisher-Yates Shuffle
 * NOTA BENE: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#Implementation_errors
 */
static uint16_t randomInt(uint16_t upperLimit) {
  uint16_t cap = RAND_MAX - (RAND_MAX % upperLimit);
  uint16_t result;
  do {
    uint32_t rando;
    HAL_RNG_GenerateRandomNumber(&hrng, &rando);
    result = (uint16_t)rando; // Could be more efficient if I didn't discard 16 bits every time?
  } while (result >= cap);
  return (result / (cap / upperLimit)); // Is this faster than "return result%upperlimit"?
}
void shuffle(void) {
  for (uint16_t i = deckSize-1; i>0; i--) {
    uint16_t j = randomInt(i+1);
    int temp = deck[i];
    deck[i] = deck[j];
    deck[j] = temp;
  }
}

void initDeck(void) {
  for (uint16_t i = 0; i < deckSize; i++) {
    deck[i] = i + 1;
  }
  shuffle();
  deckPointer = 0;
}

//void dealCard(CardSlot *slot) {
//  slot->cardVal = deck[deckPointer];
//  deckPointer++;
//}
