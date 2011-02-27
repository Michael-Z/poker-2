/*
* Creation Date : 24-02-2011

* Purpose : Compute Hand Value

* Creation Date :

* Last Modified : Sat 26 Feb 2011 03:50:42 PM EST

* Created By : Yuchen Zhou

*/
#ifndef __handValue_H
#define __handValue_H

#include <stdio.h>
#include "../game.h"

enum SUITE{
	club,
	diamond,
	heart,
	spade
};

typedef struct{
	int rank;
	enum SUITE suite;
} Card;


int rankHand(Card* myCards, int numberOfCards);
		     
float computeHandValue(Game *game, State *state, int currentPlayer, int min, int max);

#endif
