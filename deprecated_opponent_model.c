/*
* File Name : opponent_model.c

* Purpose :

* Creation Date : 23-02-2011

* Last Modified : Sat 26 Feb 2011 01:39:02 AM EST

* Created By : Weikeng Qin (weikqin@gmail.com)

*/

/* TODO:
	record at each betting round, the opponent's decision (fold, check, bet, raise)
	calculate the fraction of each decision
	save the action sequence vs. decision distribution mapping in appropriate data structure
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <getopt.h>
#include "game.h"

#define MAX_NUM_MODEL_ENTRIES	128
#define MAX_NUM_BUCKETS

typedef int Bucket;

typedef struct {
	Action actionList[ MAX_NUM_ACTIONS ];
	Bucket bucket[MAX_NUM_BUCKETS];
} strengthBaseEnt;

typedef struct {
	Action actionList[ MAX_NUM_ACTIONS ];
	int	fCnt;
	int cCnt;
	int rCnt;
} probBaseEnt;

typedef struct {
	strengthBaseEnt strengthBase[MAX_NUM_MODEL_ENTRIES];
	probBaseEnt	probBase[MAX_NUM_MODEL_ENTRIES ];
	unsigned strengthBaseDepth;
	unsigned probBaseDepth;
} OppBase;

OppBase preflopBase, flopBase, turnBase, riverBase;

/* ========= All the following functions are now only for two people, limit game =================*/

void initStrengthBaseEnt(strengthBaseEnt *ent)
{
	int i;
	for(i=0; i<MAX_NUM_ACTIONS; i++) {
		ent->actionList[i].type = invalid;
		ent->actionList[i].size = 0;
		}
	for(i=0; i<MAX_NUM_BUCKETS; i++) {
		ent->bucket[i] = 0;
		}
}

void initProbBaseEnt(probBaseEnt *ent)
{
	int i;
	for(i=0; i<MAX_NUM_ACTIONS; i++) {
		ent->actionList[i].type = invalid;
		ent->actionList[i].size = 0;
		}
	ent->fCnt = 0;
	ent->cCnt = 0;
	ent->rCnt = 0;
}
	
void initOppBase()
{
	int i = 0;
	for(i=0; i<MAX_NUM_MODEL_ENTRIES; i++) {
		initStrengthBaseEnt(preflopBase.strengthBase[i]);
		initStrengthBaseEnt(flopBase.strengthBase[i]);
		initStrengthBaseEnt(turnBase.strengthBase[i]);
		initStrengthBaseEnt(riverBase.strengthBase[i]);
		
		initProbBaseEnt(preflopBase.probBase[i]);
		initProbBaseEnt(flopBase.probBase[i]);
		initProbBaseEnt(turnBase.probBase[i]);
		initProbBaseEnt(riverBase.probBase[i]);

		preflopBase.strengthBaseDepth = 0;
		preflopBase.probBaseDepth = 0;	
		
		flopBase.strengthBaseDepth = 0;
		flopBase.probBaseDepth = 0;		
		
		turnBase.strengthBaseDepth = 0;
		turnBase.probBaseDepth = 0;		
		
		riverBase.strengthBaseDepth = 0;
		riverBase.probBaseDepth = 0;

		}
}

void updatePreflopBase(uint8_t oppPos, State state, uinit8_t pos)
{
	int i;
	int j;
	Action localActionList[MAX_NUM_ACTIONS];
	uint8_t holeCards[2];

	/* extract actions in preflop stage */
	memcpy(localActionList, state.action[0], MAX_NUM_ACTIONS * sizeof(Action));

	/* extract holeCards */
	holeCards[0] = state.holeCards[pos][0];
	holeCards[1] = state.holeCards[pos][1];
	
	if (pos == 0) { /* non-dealer */
		/* update strengthBase */
		for(i=0; i<MAX_NUM_ACTIONS; i+=2) {
			
			}
		/* update probBase */
		for(i=1; i<MAX_NUM_ACTIONS; i+=2) {
			switch (localActionList[i]) 
			{
				case 'f':
					Action tmpActList[MAX_NUM_ACTIONS];
					for(j=0; j<MAX_NUM_ACTIONS; j++) {
						tmpActList[j].type = invalid;
						}
					/* find the action list exists */
					for(j=0; j<preflopBase.probBaseDepth; i++) {
						
				case 'c':
				case 'r':
			}
		}
	}
	else if (pos == 1) { /* dealer */
	}
	else {
		fprintf(stderr, "updateOppBase: invalide player position\n");
		exit(EXIT_FAILURE);
	}	
}

/* called when a showdown message is received, only support 2-player for now */
void updateOppBase(MatchState state)
{
	//TODO: parse message from server, only useful if it's a showdown message
	//TODO: iterate through each betting round
	uint8_t oppPos = state.viewingPlayer;


	updatePreflopBase;
	updateFlopBase;
	updateTurnBase;
	updateRiverBase;
}
	/* preflop */
	
	int i, j, k;
	for(i=0, j=0, k=0; i<state.round; i++) {
		for (j=0; j<numActions[i]; j++) {
			Oaction[k] = state.action[i][j];
			k++;
		}
	}
	opponentHoleCard = state;
	opponentActionTable = {handStrengh, action}
	

	//TODO: find out a list of actions performed in each round, as well as the opponent's hand strengh
}
	
