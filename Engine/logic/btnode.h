#ifndef _BTNODE_INC
#define _BTNODE_INC

#include <string>
#include <vector>

using namespace std;

class bt;

class btnode;

enum {
	RANDOM
	, SEQUENCE
	, PRIORITY
	, ACTION
	, DECORATOR
	, DECORATOR_LUA
};

#define RANDOM 0
#define SEQUENCE 1
#define PRIORITY 2
#define ACTION 3
#define DECORATOR 4
#define DECORATOR_LUA 5

enum {
	STAY
,	OK
,	KO
};


class btnode
	{
	string name;
	int type;
	vector<btnode *>children;
	btnode *parent;
	btnode *right;
	public:
		btnode(string);
		void create(string);
		bool isRoot();
		void setParent(btnode *);
		void setRight(btnode *);
		void addChild(btnode *);
		void setType(int );
		void recalc(bt *);
		string getName();
};

#endif