/*
*	Author: Yikan Chen
*	Purpose: Primary functions of the game tree
*	Date Created:	11/02/26
*/

#include "gameTree.h"

#define MAXNODE 450		//larger than 243+81+...
#define MAXDEGREE 6		//At most 6 depth tree



Gametree* constructTree(Game *game, State *state, int currentStage, int opponentID, int selfID)		
//Construct a game tree in certain stages. If the current stage does not change, we can always determine next action with this tree
{
	uint8_t currentRound;		//current round
	uint8_t numRaise;		//number of raises allowed in this game
	DataType handStrength;
	bool isFirst = false;

	
	currentRound = state->round;
	numRaise = game->maxRaises[currentRound];
	handStrength = computeHandStrength(Game *game, State *state);
	if ((int)(game->firstplayer[currentRound]) == selfID)
		isFirst = true;
	Gametree thisGame = initTree(numRaise);
	thisGametree = computeTreevalue(game, state, thisGametree, numRaise, handStrength, opponentID, isFirst);
	return thisGametree;
}



void main()
{

	Gametree* ok;
	ok = initTree(2);

	int test = 0;
	test++;
	test++;

}

Gametree* initTree(int numRaise)
//Generate the structure of the tree
{
	//First generate a full tree and then cut it to the gametree.
	int raisenumber = 0;
	int maxlevel = (int)numRaise*2+2;		//maxlevel of the tree is combining calls/raises
	int level = 1;					//the level of the tree
	int front = 0;
	int	rear = 1;
	int i = 0;
	int temprear = rear;
	Gametree* rootnode = (Gametree *)malloc(sizeof(Gametree));	
	
	Gametree *nodeindex[MAXNODE];
	for (i = 0; i < MAXNODE; i++)
		nodeindex[i] = NULL;

	nodeindex[0] = rootnode;
	rootnode->data = 0;
	rootnode->parent = NULL;
	rootnode->raise = NULL;
	rootnode->fold = NULL;
	rootnode->call = NULL;

	level++;		//start by generating the 2nd level

	while (level <= maxlevel)
	{
		while (front < rear)
		{
			Gametree* foldnode = (Gametree *)malloc(sizeof(Gametree));


			foldnode->data = 0;
			foldnode->nodeType = 0;
			foldnode->raise = NULL;
			foldnode->call = NULL;
			foldnode->fold = NULL;
			foldnode->parent = nodeindex[front];
			(*(nodeindex[front])).fold = foldnode;
			nodeindex[temprear] = foldnode;
			temprear++;

			Gametree* callnode = (Gametree *)malloc(sizeof(Gametree));
			callnode->data = 0;
			callnode->nodeType = 1;
			callnode->raise = NULL;
			callnode->call = NULL;
			callnode->fold = NULL;
			(*(nodeindex[front])).call = callnode;
			callnode->parent = nodeindex[front];
			nodeindex[temprear] = callnode;
			temprear++;

			Gametree* raisenode = (Gametree *)malloc(sizeof(Gametree));
			raisenode->data = 0;
			raisenode->nodeType = 2;
			raisenode->raise = NULL;
			raisenode->call = NULL;
			raisenode->fold = NULL;
			(*(nodeindex[front])).raise = raisenode;
			raisenode->parent = nodeindex[front];
			nodeindex[temprear] = raisenode;
			temprear++;

			front++;
		}
		rear = temprear;
		level++;
	}

	//Begin to delete irreasonable nodes

	for (i = 0; i < rear; i++)
	{
		if (nodeindex[i]->fold)
		{		
			if ((nodeindex[i]->fold)->fold)	//all childs of fold child should be deleted
			{
				nodeindex[i]->fold->fold = NULL;
				nodeindex[i]->fold->call = NULL;
				nodeindex[i]->fold->raise = NULL;
			}
		}

		if	(nodeindex[i]->call)
		{
			if ((nodeindex[i]->call)->call)
			{
				if (((nodeindex[i]->call)->call)->fold)		//all tree nodes after 2 consecutive calls should be deleted
				{
					((nodeindex[i]->call)->call)->fold = NULL;
					((nodeindex[i]->call)->call)->call = NULL;
					((nodeindex[i]->call)->call)->raise = NULL;
				}
			}
		}
	}
	free(nodeindex);
	return rootnode;
}

Gametree* computeTreevalue(Game* game, State* state, Gametree* emptyTree, int numRaise, DataType handStrength, int opponentID, bool isFirst)
//Give all the values in the tree
{
	int nodeDegree;											//the degree of certain node
	int totalSpent, playerSpent;								//total number of chips in the pot and the chip player A spent
	int maxlevel = 2*numRaise+2;
	int level = 1;
	int front = 0;
	int	rear = 1;
	int i = 0;
	int temprear = rear;
	Gametree *nodeindex[MAXNODE];
	for (i = 0; i < MAXNODE; i++)
		nodeindex[i] = NULL;
	
	nodeindex[0] = emptyTree;
	level++;

	while (level <=maxlevel)
	{
		while (front < rear)
		{
			if ((nodeindex[front]->call) && (nodeindex[front]->fold) && (nodeindex[front]->raise))
			{
				nodeindex[temprear] =  nodeindex[front]->fold;
				nodeindex[temprear+1] =  nodeindex[front]->call;				
				nodeindex[temprear+2] =  nodeindex[front]->raise;
				temprear +=3;
			}
			front++;
		}
		rear = temprear;
		level++;
	}

	rear--;												//rear will point to the last node in the node list
	
	for(i = 0; i < MAXNODE; i++)
	{
		if (!(nodeindex[i]))							//Arrives the end of the node list
			break;
		if (!(nodeindex[i]->fold))						//This node has no child
		{
			nodeDegree = getDegree(nodeindex[i]);
			totalSpent = totalSpent(game, state, nodeindex[i], &playerSpent, isFirst);
			if (((!nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
			//If player A will decide first, then all nodes in even level will represent results of A's move, otherwise all nodes in odd level will represent results of  A's move
			{
				if (nodeindex[i]->nodeType == 0)	//a fold node
					nodeindex[i]->data = 0 - playerSpent;
				if (nodeindex[i]->nodeType == 1)	//After two calls
					nodeindex[i]->data = winningProb(game, state, handStrength, opponentID)*totalSpent-playerSpent;
			}
			else								//B's move
			{
				if (nodeindex[i]->nodeType == 0)	//a fold node
					nodeindex[i]->data = totalSpent - playerSpent;
				if (nodeindex[i]->nodeType == 1)	//After two calls
					nodeindex[i]->data = winningProb(game, state, handStrength, opponentID)*totalSpent-playerSpent;
			}
		}
	}

	for (i = rear; i >= 0; i--)
	{
		nodeDegree = getDegree(nodeindex[i]);
		if (nodeDegree == 1)
			break;
		DataType* opponentAction;
		if ((nodeindex[i]->parent == nodeindex[i-1]->parent) && (nodeindex[i-1]->parent == nodeindex[i-2]->parent))
			//Their parents will be determined by all the fold, call and raise operation values
		{
			if (((!nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
				nodeindex[i]->parent->data = max(nodeindex[i]->data,nodeindex[i-1]->data,nodeindex[i-2]->data);
			else
			{
				opponentAction=getOpponentaction(game,state);
				nodeindex[i]->parent->data = (*(opponentAction+2)) * nodeindex[i]->data + (*(opponentAction+1)) * nodeindex[i-1]->data + (*opponentAction) * nodeindex[i-2]->data;
			}
			i -= 3;
		}
		else
			//Their parents will only be determined by fold and call operations
		{
			if (((!nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
				nodeindex[i]->parent->data = (nodeindex[i]->data > nodeindex[i-1]->data) ? (nodeindex[i]->data :nodeindex[i]->data);
			else
			{
				opponentAction=getOpponentaction(game,state);
				nodeindex[i]->parent->data = (*(opponentAction+1)) * nodeindex[i]->data + (*(opponentAction)) * nodeindex[i-1]->data;
			}
			i -= 2;
		}
	}

}

int getDegree(Gametree* testnode)				//return the degree of any node, root node will be in level 1
{
	int degree = 1;
	if (!(testnode->parent))
		return 1;
	else
	{
		while (testnode->parent)
		{
			testnode = testnode->parent;
			degree++;
		}
		return degree;
	}
}

int totalSpent(Game *game, State *state, Gametree *testnode, int* playerSpent, bool isFirst)
{
	int totalSpent = state->spent[0]+state->spent[1];
	int currentCall = 0;
	int countchips[MAXDEGREE];
	int i = 0;
	int j = 0;
	//for (i=0;i<MAXDEGREE;i++)
	//	countchips[i] = 10;					//Irreasonable number 

	while (testnode->parent)
	{
		countchips[i] = testnode->nodeType;
		testnode = testnode->parent;
		i++;
	}	
	i--;

	if (isFirst)				//Player A plays first, j = 0, then all j%2==0 implies it is A's move
	{
		j = 0;
		(*playerSpent) = state->spent[0];
	}
	else
	{
		j = 1;
		(*playerSpent) = state->spent[1];
	}
	
	if (state->round == 0)		//Containing blind
	{
		if (getDegree(testnode) == 1)
			return game->blind[0] + game->blind[1];
		else
		{
			if (i == 0)								//Fold directly
			{
				if (j == 0)							//Player A folds, then Player A pays the small blind
					(*playerSpent) = game->blind[1];
				else								//Player B folds, then Player A pays the big blind
					(*playerSpent) = game->blind[2];
				return game->blind[0] + game->blind[1];

			}
			totalSpent = game->blind[0];						//big blind
			currentCall = game->blind[0];
			while (i>=0)
			{
				if (countchips[i] == 0)				//fold, exit
					return totalSpent;				
				if (countchips[i] == 1)				//call
				{
					if (j%2 == 0)					//Player A calls
						(*playerSpent) += currentCall;
					totalSpent+= currentCall;
				}
				if (countchips[i] == 2)				//raise
				{
					currentCall *= 2;				//TODO: Check raise is 2*currentcall or raise[2]?
					if (j%2 == 0)					//Player A raises
						(*playerSpent) += currentCall;
					totalSpent += currentCall
				}
				i--;
				j++;
			}
		return totalSpent;
	}
	else						//After community cards are dealt
	{
		while (i>=0)
		{
			if (countchips[i] == 0)				//fold, exit
				return totalSpent;				
			if (countchips[i] == 1)				//call
			{
				if (j%2 == 0)					//Player A calls
					(*playerSpent) += currentCall;
				totalSpent+= currentCall;
			}
			if (countchips[i] == 2)				//raise
			{
				currentCall *= 2;				//TODO: Check raise is 2*currentcall or raise[2]?
				if (j%2 == 0)					//Player A raises
					(*playerSpent) += currentCall;
				totalSpent += currentCall
			}
			i--;
			j++;
		}
		return totalSpent;
	}
}

DataType max(DataType x1, DataType x2, DataType x3)
{
	DataType max = (x1 > x2) ? (x1 : x2);
	max = (max > x3) ? (max : x3);
	return max;
}

Action* decideAction(Gametree* thisGametree, Action* actionList)
{
	//TODO: from the actionList, return the best action.
}
