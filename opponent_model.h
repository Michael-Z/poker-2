/*
* File Name : opponent_model.h

* Purpose :

* Creation Date : 23-02-2011

* Last Modified : Sun 27 Feb 2011 01:23:26 AM EST

* Created By : Weikeng Qin (weikqin@gmail.com)

*/
#define MAX_NUM_BUCKETS 128

typedef enum {false = 0, true = 1} bool;
enum NodeType {null = 0, strength = 1, prob = 2}; 

typedef struct {
	unsigned fCnt;
	unsigned cCnt;
	unsigned rCnt;
} ActionDist;


typedef unsigned Bucket;

struct Node {
	enum NodeType type;
	union {
		ActionDist actionDist;
		Bucket bucket[MAX_NUM_BUCKETS];
	} data;
#ifdef DEBUG 
	enum ActionType actionList[MAX_NUM_ACTIONS];	
	uint8_t actionNum;
#endif 
	struct Node *leftChild;
	struct Node *midChild;
	struct Node *rightChild;
};
		

typedef struct {
	struct Node *dealerRoot;
	struct Node *nonDealerRoot;
} OppBase;

extern OppBase preflopBase, flopBase, turnBase, riverBase;

unsigned calcStrength();
/* ========= All the following functions are now only for two people, limit game =================*/
void initNode(struct Node* node, enum NodeType type);
struct Node* initBase(OppBase *base, bool isPlayFirst);
void updateBase(uint8_t round, uint8_t pos, State *state);
void updateModel(uint8_t pos, State *state);
void releaseNode(struct Node *node);
void releaseBase(OppBase *base);
void initModel();

#ifdef DEBUG
void printNode(struct Node *node);
void printBase(OppBase *base);
void printModel();
#endif

