/*
 * deck.h
 *
 *  Created on: May 6, 2022
 *      Author: Sam
 */

#ifndef SRC_PROSET_DECK_H_
#define SRC_PROSET_DECK_H_

#define MAX_CARDS 512 // A deck at level 9 would have 2^9-1 (511) cards in it

void shuffle(void);
void initDeck(void);

#endif /* SRC_PROSET_DECK_H_ */
