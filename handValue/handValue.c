/*
* Creation Date : 24-02-2011

* Purpose :

* Creation Date :

* Last Modified : Fri 25 Feb 2011 11:30:31 AM EST

* Created By :

*/

#include "handValue.h"
#include <stdio.h>
#include <stdlib.h>

void computeHandValue(Game *game, State *state, int currentPlayer, int min, int max)
{
	Card *myCards = (Card *) (malloc(sizeof(Card)*5));
	myCards[0].rank = state->holeCards[currentPlayer][0]/4+2;
	myCards[1].rank = state->holeCards[currentPlayer][1]/4+2;
	myCards[0].suite = state->holeCards[currentPlayer][0]%4;
	myCards[1].suite = state->holeCards[currentPlayer][1]%4;
	if (state->round == 0)			//pre-flop
	{
	}
	else							//post-flop
	{

	}
	FILE *fp;
	fp = fopen("output.txt","a+");
	fprintf(fp,"%d\n", currentPlayer);
	fprintf(fp,"My first hole card is %d, suite is %d; Second hole card is %d, suite is %d\n", myCards[0].rank, myCards[0].suite, myCards[1].rank, myCards[1].suite);
	if (state->round > 0)
	{
		fprintf(fp, "community cards are: %d, %d, %d, %d, %d\n", state->boardCards[0], state->boardCards[1], state->boardCards[2], state->boardCards[3], state->boardCards[4]);
	}
	fclose(fp);
}

