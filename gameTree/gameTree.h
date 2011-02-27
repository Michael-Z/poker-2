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

//TODO: include necessary files for hand strength and opponent model
//TODO: function needed: 
//winningProb(game, state, handStrength, opponentID);
//getOpponentaction(game,state);
//computeHandStrength(Game *game, State *state)


typedef float DataType;		//Basic datatype

//typedef	struct _treeNode*	Gametree;//game tree
typedef struct Gametree
{			
	//game tree node
	DataType	data;
	int			nodeType;		//0:fold, 1:call, 2:raise
	Gametree*	parent;
	Gametree*	fold;
	Gametree*	call;
	Gametree*	raise;
}	Gametree;

Gametree* constructTree(int currentStage, int opponentID, bool isFirst);

Gametree* initTree(int numRaise);

Gametree* computeTreevalue(Game* game, State* state, Gametree* emptyTree, int numRaise, DataType handStrength, int opponentID, bool isFirst)

int getDegree(Gametree* testnode);

int totalSpent(Game *game, State *state, Gametree *testnode, int* playerSpent, bool isFirst)

DataType max(DataType x1, DataType x2, DataType x3)

Action* decideAction(Gametree* thisGametree, Action* actionList);
//from the actionList, return the best action

#endif	//gameTree