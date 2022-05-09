/*
 * deck.c
 *
 *  Created on: May 6, 2022
 *      Author: Sam
 */

#include <stdlib.h>
#include "stdio.h"
#include "deck.h"
#include "card.h"
#include "serial_logging.h"

/* Globals for difficulty level, deck stuff */

extern uint16_t numDots;
extern uint16_t deckSize;
Card deck[MAX_CARDS];

/*
 * Fisher-Yates Shuffle
 * NOTA BENE: https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#Implementation_errors
 */
static void swapCards(Card *a, Card *b) {
  Card temp = *a;
  *a = *b;
  *b = temp;
}
static uint16_t randomInt(uint16_t upperLimit) {
  uint16_t cap = RAND_MAX - (RAND_MAX % upperLimit);
  uint16_t result;
  do {
    result = rand();
  } while (result >= cap);
  return (result / (cap / upperLimit)); // Is this faster than "return result%upperlimit"?
}
void shuffle(void) {
  for (uint16_t i = deckSize-1; i>0; i--) {
    uint16_t j = randomInt(i+1);
    swapCards(&deck[i], &deck[j]);
  }
}

void initDeck(void) {
  for (uint16_t i = 0; i < deckSize; i++) {
    deck[i].value = i + 1;
  }
  shuffle();
}
