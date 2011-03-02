/*
* File Name : opponent_model.c

* Purpose :

* Creation Date : 26-02-2011

* Last Modified : Wed 02 Mar 2011 05:39:06 PM EST

* Created By : Weikeng Qin (weikqin@gmail.com)

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
#include "opponent_model.h"
#include "handValue/handValue.h"

	
OppBase flopsBase[MAX_ROUNDS];

/* ========= All the following functions are now only for two people, limit game =================*/
void initNode(struct Node* node, enum NodeType type)
{
	node->type = type;
	node->leftChild = NULL;
	node->midChild = NULL;
	node->rightChild = NULL;
#ifdef DEBUG
	node->actionNum = 0;
#endif
	if (strength == type) {
		node->data.actionDist.fCnt = 0;
		node->data.actionDist.cCnt = 0;
		node->data.actionDist.rCnt = 0;
	}
	else {
		memset(node->data.bucket, 0, sizeof(Bucket)*MAX_NUM_BUCKETS);
	}
}

struct Node* initBase(OppBase *base, bool isPlayFirst)
{
	if (false == isPlayFirst) {
		base->nonDealerRoot  = (struct Node*)malloc(sizeof(struct Node));
		initNode(base->nonDealerRoot, strength);
		return base->nonDealerRoot;
		}
	else {
		base->dealerRoot  = (struct Node*)malloc(sizeof(struct Node));
		initNode(base->dealerRoot, prob);
		return base->dealerRoot;
	}
}

void initModel(Game *game)
{
	int i;
	for (i=0; i<game->numRounds; i++) {
		flopsBase[i].dealerRoot = NULL;
		flopsBase[i].nonDealerRoot = NULL;
	}
}

void updateBase(uint8_t round, uint8_t pos, State *state)
{
	int k;
	Action localActionList[MAX_NUM_ACTIONS];
	uint8_t localActionNum;
	uint8_t holeCards[2];
	struct Node *pt = NULL;
	OppBase *base;

	if (round < MAX_ROUNDS) {
		base = &flopsBase[round];
	}
	else {
		fprintf(stderr, "updateBase: invalid round argument %d\n", round);
		exit(EXIT_FAILURE);
	}
	
	/* extract actions in  stage */
	memcpy(localActionList, state->action[round], state->numActions[round]* sizeof(Action));
	localActionNum = state->numActions[round];
	#if 0
	/* debug */
		printf("Actions this round\n");
		for(k=0; k<localActionNum; k++) printf("%d\t", localActionList[k].type);
		printf("\n");
	#endif

	/* extract holeCards */
	holeCards[0] = state->holeCards[pos][0];
	holeCards[1] = state->holeCards[pos][1];

	if (0 == round) {/* preflop */
		if (pos == 0) { /* non-first hand*/
			pt = (NULL == base->nonDealerRoot ? initBase(base, false): base->nonDealerRoot);
		}
		else if (pos == 1) { /* first hand*/
			pt = (NULL == base->dealerRoot ? initBase(base, true): base->dealerRoot);
		}
		else {
			fprintf(stderr, "updateOppBase: invalid player position\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		if (pos == 0) { /* first player*/
			pt = (NULL == base->dealerRoot ? initBase(base, true): base->dealerRoot);
		}
		else if (pos == 1) { /* non-first player*/
			pt = (NULL == base->nonDealerRoot ? initBase(base, false): base->nonDealerRoot);
		}
		else {
			fprintf(stderr, "updateOppBase: invalid player position\n");
			exit(EXIT_FAILURE);
		}
	}
	
	for(k=0; k<localActionNum; k++) {
		#if 0
		if (k>0) { 
			pt->actionList[k-1] = localActionList[k-1].type; 
			pt->actionNum ++;
		}
		#endif
		
		if (pt->type == prob) { /* update  prob info */
			switch (localActionList[k].type) {
				case fold:
					pt->data.actionDist.fCnt++;
					if (pt->leftChild == NULL) {
						pt->leftChild = malloc(sizeof(struct Node));
						initNode(pt->leftChild, strength);
#ifdef DEBUG
						memcpy(pt->leftChild->actionList, pt->actionList, sizeof(int) * pt->actionNum);
						pt->leftChild->actionNum = pt->actionNum + 1;
						pt->leftChild->actionList[pt->leftChild->actionNum-1] = localActionList[k].type;
#endif
					}
					pt = pt->leftChild;
					break;
				case call:
					pt->data.actionDist.cCnt++;
					if (pt->midChild == NULL) {
						pt->midChild = malloc(sizeof(struct Node));
						initNode(pt->midChild, strength);
#ifdef DEBUG
						memcpy(pt->midChild->actionList, pt->actionList, sizeof(int) * pt->actionNum);
						pt->midChild->actionNum = pt->actionNum + 1;
						pt->midChild->actionList[pt->midChild->actionNum-1] = localActionList[k].type;
#endif
						}
					pt = pt->midChild;
					break;
				case raise:
					pt->data.actionDist.rCnt++;
					if (pt->rightChild == NULL) {
						pt->rightChild = malloc(sizeof(struct Node));
						initNode(pt->rightChild, strength);
#ifdef DEBUG
						memcpy(pt->rightChild->actionList, pt->actionList, sizeof(int) * pt->actionNum);
						pt->rightChild->actionNum = pt->actionNum + 1;
						pt->rightChild->actionList[pt->rightChild->actionNum-1] = localActionList[k].type;
#endif
						}
					pt = pt->rightChild;
					break;
				default:
					fprintf(stderr, "updateBase: invalid action %d\n", localActionList[k].type);
					exit(EXIT_FAILURE);
				}
		}
		else {
			unsigned idx = computeHandStrength(state,pos);
			pt->data.bucket[idx] ++;
			switch (localActionList[k].type) { /* update strength info */
				case fold:
					if (pt->leftChild == NULL) {
						pt->leftChild = malloc(sizeof(struct Node));
						initNode(pt->leftChild, prob);
#ifdef DEBUG
						memcpy(pt->leftChild->actionList, pt->actionList, sizeof(int) * pt->actionNum);
						pt->leftChild->actionNum = pt->actionNum + 1;
						pt->leftChild->actionList[pt->leftChild->actionNum-1] = localActionList[k].type;
#endif
						}
					pt = pt->leftChild;
					break;
				case call:
					if (pt->midChild == NULL) {
						pt->midChild = malloc(sizeof(struct Node));
						initNode(pt->midChild, prob);
#ifdef DEBUG
						memcpy(pt->midChild->actionList, pt->actionList, sizeof(int) * pt->actionNum);
						pt->midChild->actionNum = pt->actionNum + 1;
						pt->midChild->actionList[pt->midChild->actionNum-1] = localActionList[k].type;
#endif
					}
					pt = pt->midChild;
					break;
				case raise:
					if (pt->rightChild == NULL) {
						pt->rightChild = malloc(sizeof(struct Node));
						initNode(pt->rightChild, prob);
#ifdef DEBUG
						memcpy(pt->rightChild->actionList, pt->actionList, sizeof(int) * pt->actionNum);
						pt->rightChild->actionNum = pt->actionNum + 1;
						pt->rightChild->actionList[pt->rightChild->actionNum-1] = localActionList[k].type;
#endif
					}
					pt = pt->rightChild;
					break;
				default:
					fprintf(stderr, "updateBase: invalid action %d\n", localActionList[k].type);
					exit(EXIT_FAILURE);
				}
			}
	}
}

void updateModel(Game *game, uint8_t pos, State *state)
{
	int i;
	for(i=0; i<game->numRounds; i++) {
		updateBase((uint8_t)i, pos, state);
	}
}
	
struct Node *getNode(Action *act, uint8_t actLen, uint8_t round, uint8_t pos) 
{
	int i = 0;
	struct Node *node = NULL;

	if (round >= MAX_ROUNDS) {
		fprintf(stderr, "invalid pos: %d\n", pos); 
		exit(EXIT_FAILURE);
	}
	else if (0 == round) {
		if (0 == pos) { node = flopsBase[0].nonDealerRoot; }
		else if(1 == pos) { node = flopsBase[0].dealerRoot; }
		else { fprintf(stderr, "invalid pos: %d\n", pos); exit(EXIT_FAILURE);}
	}
	else {
		if (1 == pos) { node = flopsBase[round].nonDealerRoot; }
		else if(0 == pos) { node = flopsBase[round].dealerRoot; }
		else { fprintf(stderr, "invalid pos: %d\n", pos); exit(EXIT_FAILURE);}
	}
		
	for(i=0; i<actLen; i++) {
		switch ( act[i].type ) {
		case fold:
			node = node->leftChild;
			break;
		case call:
			node = node->midChild;
			break;
		case raise:
			node = node->rightChild;
			break;
		default:
			fprintf(stderr, "invalid action: %d\n", act[i].type);
			exit(EXIT_FAILURE);
		}
	}
	return node;
}
			
		
void releaseNode(struct Node *node)
{
	if (NULL == node) return;
	
	releaseNode(node->leftChild);
	releaseNode(node->midChild);
	releaseNode(node->rightChild);
	releaseNode(node);
}

void releaseBase(OppBase *base)
{
	if (NULL != base) {
		releaseNode(base->nonDealerRoot);
		releaseNode(base->dealerRoot);
	}
}

#ifdef DEBUG
void printNode(struct Node *node)
{
	if (NULL == node) return;

	fprintf(stdout, "Betting: ");
	int i;
	for(i=0; i<node->actionNum; i++) {
		switch(node->actionList[i]) 
			{
			case fold:
				fprintf(stdout, "f");
				break;
			case call:
				fprintf(stdout, "c");
				break;
			case raise:
				fprintf(stdout, "r");
				break;
			case invalid:
				break;
			default:
				fprintf(stderr, "printNode: invalid action\n");
				exit(EXIT_FAILURE);
				break;
			}
		if (node->actionList[i] == invalid) break;
	}
	fprintf(stdout, "\t");
	if (node->type == strength) {
		fprintf(stdout, "strength: %d\n", node->data.bucket[0]);
		}
	else {
		fprintf(stdout, "flop rate: %d\t, call rate: %d\t, raise rate: %d\n", node->data.actionDist.fCnt, node->data.actionDist.cCnt, node->data.actionDist.rCnt);
		}
	printNode(node->leftChild);
	printNode(node->midChild);
	printNode(node->rightChild);
}
void printBase(OppBase *base)
{
	fprintf(stdout, "Case first player\n");
	printNode(base->dealerRoot);
	fprintf(stdout, "Case nonfirst player\n");
	printNode(base->nonDealerRoot);
	printf("\n");
}

void printModel(Game *game)
{
	int i;
	for(i=0; i<game->numRounds; i++) {
		printBase(&flopsBase[i]);
	}
}
#endif
