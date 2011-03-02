/*
*	Author: Yikan Chen
*	Purpose: Head file for the game tree structure
*	Date Created:	11/02/26
*/

#ifndef	GAMETREE
#define	GRMETREE

#include	<stdlib.h>
#include	<stdio.h>
#include	<malloc.h>
#include	"game.h"
//need to modify rng.h as I did in VS2008, not sure if it is necessary in gcc


//TODO: include necessary files for hand strength and opponent model
//TODO: function needed: 
//winningProb(game, state, handStrength, opponentID);
//getOpponentaction(game,state);
//computeHandStrength(Game *game, State *state)





//typedef	struct _treeNode*	Gametree;//game tree
//typedef struct GametreeNode Gametree;

typedef float DataType;		//Basic datatype
typedef int bool;

typedef struct Gametree
{			
	//game tree node
	DataType	data;	
	int			nodeType;		//0:fold, 1:call, 2:raise
	struct Gametree*	parent;
	struct Gametree*	fold;
	struct Gametree*	call;
	struct Gametree*	raise;
}	Gametree;

//functions


Gametree* initTree(int numRaise);

Gametree* constructTree(Game *game, State *state, int opponentID, int selfID);	

Gametree* computeTreevalue(Game* game, State* state, Gametree* emptyTree, int numRaise, DataType handStrength, int opponentID, bool isFirst);

int getDegree(Gametree* testnode);

int totalSpentChips(Game *game, State *state, Gametree *testnode, int* playerSpent, bool isFirst);

Action* getActionList(Gametree *testnode);

DataType findMax(DataType x1, DataType x2, DataType x3);

Action* decideAction(Gametree* thisGametree, Action* actionList, int actionNumber);


//Naive functions for debugging
DataType computeHandStrength(Game *game, State *state);

DataType winningProb(Game* game, State* state, DataType handStrength,int opponentID);

DataType* getOpponentaction(Game* game,State* state);


//from the actionList, return the best action

#endif	//gameTree