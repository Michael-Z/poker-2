/*
* Creation Date : 24-02-2011

* Purpose :

* Creation Date :

* Last Modified : Fri 25 Feb 2011 05:45:32 PM EST

* Created By :

*/

#include "handValue.h"
#include "../game.h"
#include "../evalHandTables"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char cardToChar (int rank)
{
	char *ret = (char *) malloc(sizeof(char)*2);
	if ((rank>0)&&(rank < 10)) sprintf(ret, "%d", rank);
	else if (rank == 10) return 'T';
	else if (rank == 11) return 'J';
	else if (rank == 12) return 'Q';
	else if (rank == 13) return 'K';
	else if (rank == 14) return 'A';
	return ret[0];
}

int computePreFlop(Card *myCards)
{
	//This value does not count what the opponent model is.
	//Using Billing's thesis' table:
	int suited = (myCards[0].suite == myCards[1].suite);
	//int pair = (myCards[0].rank == myCards[1].rank);
	char *cards = (char *) malloc(sizeof(char)*3);
	//Represent the cards using string.
	if (myCards[0].rank < myCards[1].rank)		//sort the hands
	{
		cards[0] = cardToChar(myCards[1].rank+2);
		cards[1] = cardToChar(myCards[0].rank+2);
		cards[2] = suited ? 's' : 'o';
	}
	else
	{
		cards[0] = cardToChar(myCards[0].rank+2);
		cards[1] = cardToChar(myCards[1].rank+2);
		cards[2] = suited ? 's' : 'o';
	}
	//Group1 hands:
	if ((strcmp(cards,"AAo"))==0||(strcmp(cards,"KKo"))==0||(strcmp(cards,"QQo"))==0||(strcmp(cards,"JJo"))==0||(strcmp(cards,"AKs"))==0)
	{
		return 10;		//maximum group
	}
	//Group2 hands:
	if ((strcmp(cards,"TTo"))==0||(strcmp(cards,"AQs"))==0||(strcmp(cards,"AJs"))==0||(strcmp(cards,"KQs"))==0||(strcmp(cards,"AKo"))==0)
	{
		return 9;
	}
	//Group3 hands:
	if ((strcmp(cards,"99o"))==0||(strcmp(cards,"JTs"))==0||(strcmp(cards,"QJs"))==0||(strcmp(cards,"KJs"))==0||(strcmp(cards,"ATs"))==0||(strcmp(cards,"ATo"))==0)
	{
		return 8;
	}
	//Group4 hands:
	if ((strcmp(cards,"T9s"))==0||(strcmp(cards,"KQo"))==0||(strcmp(cards,"88o"))==0||(strcmp(cards,"QTs"))==0||(strcmp(cards,"98s"))==0||(strcmp(cards,"J9s"))==0||(strcmp(cards,"AJo"))==0||(strcmp(cards,"KTs"))==0)
	{
		return 7;
	}
	//Group5 hands:
	if ((strcmp(cards,"77o"))==0||(strcmp(cards,"87s"))==0||(strcmp(cards,"Q9s"))==0||(strcmp(cards,"T8s"))==0||(strcmp(cards,"KJo"))==0||(strcmp(cards,"QJo"))==0||(strcmp(cards,"JTo"))==0||(strcmp(cards,"76s"))==0 \
	||(strcmp(cards,"97s"))==0||(strcmp(cards,"A9s"))==0||(strcmp(cards,"A8s"))==0||(strcmp(cards,"A7s"))==0||(strcmp(cards,"A6s"))==0||(strcmp(cards,"A5s"))==0||(strcmp(cards,"A4s"))==0||(strcmp(cards,"A3s"))==0|| \
	(strcmp(cards,"A2s"))==0||(strcmp(cards,"65s"))==0)
	{
		return 6;
	}
	//Group6 hands:
	if ((strcmp(cards,"66o"))==0||(strcmp(cards,"ATo"))==0||(strcmp(cards,"55o"))==0||(strcmp(cards,"86s"))==0||(strcmp(cards,"KTo"))==0||(strcmp(cards,"QTo"))==0||(strcmp(cards,"54s"))==0||(strcmp(cards,"K9s"))==0 \
	||(strcmp(cards,"J8s"))==0)
	{
		return 5;
	}
	//Group7 hands:
	if ((strcmp(cards,"44o"))==0||(strcmp(cards,"J9o"))==0||(strcmp(cards,"43s"))==0||(strcmp(cards,"75s"))==0||(strcmp(cards,"T9o"))==0||(strcmp(cards,"33o"))==0||(strcmp(cards,"98o"))==0||(strcmp(cards,"64s"))==0 \
	||(strcmp(cards,"22o"))==0||(strcmp(cards,"Q8s"))==0||(strcmp(cards,"K8s"))==0||(strcmp(cards,"K7s"))==0||(strcmp(cards,"K6s"))==0||(strcmp(cards,"K5s"))==0||(strcmp(cards,"K4s"))==0||(strcmp(cards,"K3s"))==0|| \
	(strcmp(cards,"K2s"))==0)
	{
		return 4;
	}
	//Group8 hands:
	if ((strcmp(cards,"87o"))==0||(strcmp(cards,"53s"))==0||(strcmp(cards,"A9o"))==0||(strcmp(cards,"Q9o"))==0||(strcmp(cards,"76o"))==0||(strcmp(cards,"42s"))==0||(strcmp(cards,"32s"))==0||(strcmp(cards,"96s"))==0 \
	||(strcmp(cards,"85s"))==0||(strcmp(cards,"J8o"))==0||(strcmp(cards,"J7s"))==0||(strcmp(cards,"65o"))==0||(strcmp(cards,"54o"))==0||(strcmp(cards,"74s"))==0||(strcmp(cards,"K9o"))==0||(strcmp(cards,"T8o"))==0)
	{
		return 3;
	}
	return 1;
}

int rankMyHand(Card* myCards, int numberOfCards)
{
	int i;
	Cardset c = emptyCardset();
	for(i = 0;i < numberOfCards;++i) 
	{
		addCardToCardset( &c, myCards[i].suite, myCards[i].rank);
	}
	return rankCardset(c);
}

void computeHandValue(Game *game, State *state, int currentPlayer, int min, int max)
{
	int i = 0;
	int maxNumberOfCards = 7;
	int handValue = -1;
	Card *myCards = (Card *) (malloc(sizeof(Card)*maxNumberOfCards));		//counting post-flop maximum
	for (i = 0; i < 2; i++)
	{
		myCards[i].rank = rankOfCard(state->holeCards[currentPlayer][i]);
		myCards[i].suite = suitOfCard(state->holeCards[currentPlayer][i]);
	}
	for (i = 0; i < 5; i++)
	if (state->round == 0)			//pre-flop
	{
		handValue = computePreFlop(myCards);
	}
	else							//post-flop
	{
		for (i = 0; i < 5; i++)
		{
			myCards[i+2].rank = rankOfCard(state->boardCards[i]);
			myCards[i+2].suite = suitOfCard(state->boardCards[i]);
		}
		handValue = rankMyHand(myCards, 7);
	}
	FILE *fp;
	fp = fopen("output.txt","a+");
	fprintf(fp,"This is from player %d:, betting round %d.\n", currentPlayer, state->round);
	fprintf(fp,"My first hole card is %d, suite is %d; Second hole card is %d, suite is %d\n", myCards[0].rank, myCards[0].suite, myCards[1].rank, myCards[1].suite);
	if (state->round == 0)
	{
		fprintf(fp,"Pre-flop hand strength is: %d\n", handValue);
	}
	if (state->round > 0)
	{
		fprintf(fp, "community cards are: %d, %d, %d, %d, %d\n", rankOfCard(state->boardCards[0]), rankOfCard(state->boardCards[1]), rankOfCard(state->boardCards[2]), rankOfCard(state->boardCards[3]), rankOfCard(state->boardCards[4]));
		fprintf(fp,"Post-flop hand strength is: %d\n", handValue);
	}
	fprintf(fp, "--------------------------------\n");
	fclose(fp);
}

