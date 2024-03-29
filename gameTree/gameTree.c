/*
*	Author: Yikan Chen
*	Purpose: Primary functions of the game tree
*	Date Created:	11/02/26
*/

#include "gameTree.h"

#define MAXNODE 450		//larger than 243+81+...
#define MAXDEGREE 6		//At most 6 depth tree



Gametree* constructTree(Game *game, State *state,int opponentID, int selfID)		
//Construct a game tree in certain stages. If the current stage does not change, we can always determine next action with this tree
{
	uint8_t currentRound;		//current round
	uint8_t numRaise;		//number of raises allowed in this game
	DataType handStrength;
	bool isFirst = 0;
	Gametree *thisGametree;

	
	currentRound = state->round;
	numRaise = game->maxRaises[currentRound];
	thisGametree = initTree(numRaise);
	handStrength = computeHandStrength(game,state);
	if ((int)(game->firstPlayer[currentRound]) == selfID)
		isFirst = 1;

	thisGametree = computeTreevalue(game, state, thisGametree, numRaise, handStrength, opponentID, isFirst);
	return thisGametree;
}


/*
void main()
{

	Gametree* ok = (Gametree *)malloc(sizeof(Gametree *));

	Game testgame;
	State teststate;
	int test = 0;

	testgame.maxRaises[0] = 2;
	testgame.maxRaises[1] = 2;
	testgame.firstPlayer[0] = 1;
	testgame.firstPlayer[1] = 1;
	testgame.blind[0] = 10;
	testgame.blind[1] = 5;
	testgame.raiseSize[0] = 10;
	testgame.raiseSize[1] = 20;

	teststate.round = 1 ;
	teststate.spent[0] = 30;
	teststate.spent[1] = 30;







	ok = constructTree(&testgame, &teststate, 1, 0);


	test++;
	test++;

}
*/


Gametree* initTree(int numRaise)
//Generate the structure of the tree
{
	//First generate a full tree and then cut it to the gametree.
	int raisenumber = 0;



	//---------------------------------------------------------------------------------------
	//NOTICE: Current logic about the depth of the tree:
	//Two player game, one player can only raise ceil(numRaise/2) times and from that we know 
	//the max possible depth of the tree should be: crrc for numRaise = 2, generally, it should
	//be numRaise+2 operations and numRaise+3 levels
	//---------------------------------------------------------------------------------------

	int maxlevel = (int)numRaise+3;		//maxlevel of the tree is combining calls/raises
	int level = 1;					//the level of the tree
	int front = 0;
	int	rear = 1;
	int i = 0;
	int j = 0;
	int temprear = rear;
	Gametree* rootnode = (Gametree *)malloc(sizeof(Gametree *));	
	Gametree* temp;
	
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
			Gametree* foldnode = (Gametree *)malloc(sizeof(Gametree *));
			Gametree* callnode = (Gametree *)malloc(sizeof(Gametree *));
			Gametree* raisenode = (Gametree *)malloc(sizeof(Gametree *));

			foldnode->data = 0;
			foldnode->nodeType = 0;
			foldnode->raise = NULL;
			foldnode->call = NULL;
			foldnode->fold = NULL;
			foldnode->parent = nodeindex[front];
			(*(nodeindex[front])).fold = foldnode;
			nodeindex[temprear] = foldnode;
			temprear++;


			callnode->data = 0;
			callnode->nodeType = 1;
			callnode->raise = NULL;
			callnode->call = NULL;
			callnode->fold = NULL;
			(*(nodeindex[front])).call = callnode;
			callnode->parent = nodeindex[front];
			nodeindex[temprear] = callnode;
			temprear++;


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

	for (i = 0; i < rear; i++)		//0~rear will be all the nodes
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
		
		if (nodeindex[i]->raise)							//calls after a raise must terminate the game
		{
			if ((nodeindex[i]->raise)->call)
			{
				(nodeindex[i]->raise)->call->fold = NULL;
				(nodeindex[i]->raise)->call->call = NULL;
				(nodeindex[i]->raise)->call->raise = NULL;
			}

			j = 1;
			temp = nodeindex[i]->raise;
			while((j < numRaise) && temp)					//there will be at most numRaise raises
			{
				temp = temp->raise;
				j++;
			}
			if (temp)
				temp->raise = NULL;
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
//	free(nodeindex);
	return rootnode;
}

Gametree* computeTreevalue(Game* game, State* state, Gametree* emptyTree, int numRaise, DataType handStrength, int opponentID, bool isFirst)
//Give all the values in the tree
{
	int nodeDegree;											//the degree of certain node
	int totalSpent, playerSpent;								//total number of chips in the pot and the chip player A spent
	int maxlevel = numRaise+3;
	int level = 1;
	int front = 0;
	int	rear = 1;
	int i = 0;
	int temprear = rear;
	DataType* opponentAction;
	Gametree *nodeindex[MAXNODE];
	for (i = 0; i < MAXNODE; i++)
		nodeindex[i] = NULL;
	
	nodeindex[0] = emptyTree;
	level++;

	while (level <=maxlevel)
	{
		while (front < rear)
		{
			if ((nodeindex[front]->call) && (nodeindex[front]->fold))
			{
				nodeindex[temprear] =  nodeindex[front]->fold;
				nodeindex[temprear+1] =  nodeindex[front]->call;	
				temprear +=2;
				if (nodeindex[front]->raise)
				{
					nodeindex[temprear] =  nodeindex[front]->raise;
					temprear ++;
				}
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
			totalSpent = totalSpentChips(game, state, nodeindex[i], &playerSpent, isFirst);
			if ((!(nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
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

	for (i = rear; i >= 0;)
	{
		nodeDegree = getDegree(nodeindex[i]);
		if (nodeDegree == 1)					//The data for the root node should always be computed later
			break;

		if ((nodeindex[i]->parent == nodeindex[i-1]->parent) && (nodeindex[i-1]->parent == nodeindex[i-2]->parent))
			//Their parents will be determined by all the fold, call and raise operation values
		{
			if ((!(nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
				nodeindex[i]->parent->data = findMax(nodeindex[i]->data,nodeindex[i-1]->data,nodeindex[i-2]->data);
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
			if ((!(nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
				nodeindex[i]->parent->data = ((nodeindex[i]->data) > (nodeindex[i-1]->data)) ? (nodeindex[i]->data) :(nodeindex[i]->data);
			else
			{
				opponentAction=getOpponentaction(game,state);
				nodeindex[i]->parent->data = (*(opponentAction+1)) * nodeindex[i]->data + (*(opponentAction)) * nodeindex[i-1]->data;
			}
			i -= 2;
		}
	}
	return emptyTree;
}

int getDegree(Gametree* testnode)				//return the degree of any node, root node will be in level 1
{
	int degree = 1;
	Gametree *tempnode;
	tempnode = testnode;
	if (!(tempnode->parent))
		return 1;
	else
	{
		while (tempnode->parent)
		{
			tempnode = tempnode->parent;
			degree++;
		}
		return degree;
	}
}

int totalSpentChips(Game *game, State *state, Gametree *testnode, int* playerSpent, bool isFirst)
{
	int totalSpent = state->spent[0]+state->spent[1];				//Chips already in the pot, TWO PLAYERS ONLY!
	int currentCall = 0;											//Like check, no more chips needed to check
	int countchips[MAXDEGREE];
	int i = 0;
	int j = 0;
	int temp = 0;
	Gametree* tempnode;
	tempnode = testnode;
	//for (i=0;i<MAXDEGREE;i++)
	//	countchips[i] = 10;					//Irreasonable number 

	while (tempnode->parent)						//Find how this node is reached
	{
		countchips[i] = tempnode->nodeType;
		tempnode = tempnode->parent;
		i++;
	}	
	i--;						//countchips[i] means the first action (from root)

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
			if (i == 0)								//fold directly because all the testnodes in this function must be leaf nodes
			{
				if (j == 0)							//Player A folds, then Player A pays the small blind
					(*playerSpent) = game->blind[1];
				else								//Player B folds, then Player A pays the big blind
					(*playerSpent) = game->blind[0];
				return game->blind[0] + game->blind[1];

			}
			totalSpent = game->blind[0] * 2;						//big blind
			currentCall = game->blind[0];
			*playerSpent = game->blind[0];
			while (i>=0)
			{
				if (countchips[i] == 0)				//fold, exit
				{
					if (getDegree(testnode) == 2)
					{
						if (isFirst)
							*playerSpent = game->blind[1];
						else
							*playerSpent = game->blind[0];
						totalSpent = game->blind[0] + game->blind[1];
					}
					return totalSpent;	
				}
				if (countchips[i] == 1)				//call
				{
					if (j%2 == 0)					//Player A calls
						(*playerSpent) = currentCall;
					if (countchips[i+1] == 2)		//just raised, MAYBE A PROBLEM when numRaise != 2
						totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;

				}
				if (countchips[i] == 2)				//raise
				{
					currentCall += game->raiseSize[state->round];				//after one raise, to call the player must pay extra raiseSize(currentround) chips
					if (j%2 == 0)					//Player A raises
						(*playerSpent) = currentCall;
					if (countchips[i+1] == 2)		//just raised
						totalSpent = totalSpent - (currentCall - 2 * game->raiseSize[state->round]) + currentCall;
					else
						totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;
				}
				i--;
				j++;
			}
		return totalSpent;
		}
	}
	else						//After community cards are dealt
	{
		temp = *playerSpent;
		*playerSpent = 0;
		while (i>=0)
		{
			if (countchips[i] == 0)				//fold, exit
			{
				*playerSpent = *playerSpent + temp;
				return totalSpent;
			}
			if (countchips[i] == 1)				//call
			{
				if (j%2 == 0)					//Player A calls
					(*playerSpent) = currentCall;
				if (countchips[i+1] == 2)		//just raised
					totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;
			}
			if (countchips[i] == 2)				//raise
			{
				currentCall += game->raiseSize[state->round];				//after one raise, to call the player must pay extra raiseSize(currentround) chips
				if (j%2 == 0)					//Player A raises
					(*playerSpent) = currentCall;
				if (countchips[i+1] == 2)		//just raised
					totalSpent = totalSpent - (currentCall - 2 * game->raiseSize[state->round]) + currentCall;
				else
					totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;
			}
			i--;
			j++;
		}
		*playerSpent = *playerSpent + temp;
		return totalSpent;
	}
}

Action* getActionList(Gametree *testnode)
{
	Action *actionList, *temp;
	Gametree *tempnode;
	int i = 0;
	int j = 0;

	tempnode = testnode;

	if (getDegree(testnode) == 1)
	{
		actionList = NULL;
		return actionList;
	}
	while (tempnode->parent)
	{
		(*(temp+i)).type = tempnode->nodeType;
		i++;
		tempnode = tempnode->parent;
	}
	i--;
	for (j = 0; j <= i; j++)
	{
		(*(actionList + j)).type = (*(temp + i)).type;
		i--;
	}
	return actionList;
}

DataType findMax(DataType x1, DataType x2, DataType x3)
{
	DataType maxNo = (x1 > x2) ? x1 : x2;
	maxNo = (maxNo > x3) ? maxNo : x3;
	return maxNo;
}

Action* decideAction(Gametree* thisGametree, Action* actionList, int actionNumber)
{
	//TODO: from the actionList, return the best action.
	Action action;
	Gametree* temptree = thisGametree;
	int i = 0;
	for(i = 0; i < actionNumber; i++)
	{
		if ((*(actionList+i)).type == 0)
			temptree = temptree->fold;
		else if ((*(actionList+i)).type == 1)
			temptree = temptree->call;
		else if ((*(actionList+i)).type == 2)
			temptree = temptree->raise;
	}
	if (temptree->data == temptree->fold->data)
		action.type = 0;
	else if (temptree->data == temptree->call->data)
		action.type = 1;
	else
		action.type = 2;
	return &action;
}


//Naive functions for debugging
/*
DataType computeHandStrength(Game *game, State *state)
{
	DataType k = 0.45;
	return k;
}

DataType winningProb(Game* game, State* state, DataType handStrength,int opponentID)
{
	return 0.2;
}

DataType* getOpponentaction(Game* game,State* state)
{
	DataType *opponentAction = (DataType *)malloc(sizeof(DataType *) * 3);
	*opponentAction = 0.2;
	*(opponentAction + 1) = 0.3;
	*(opponentAction + 2) = 0.5;
	return opponentAction;

}
*/
