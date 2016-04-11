#ifndef _BT_INC
#define _BT_INC

#include <string>
#include <map>
#include "btnode.h"
#include "components/components.h"

using namespace std;

typedef int (bt::*btaction)();
typedef bool (bt::*btcondition)();

// Implementation of the behavior tree
// uses the BTnode so both work as a system
// tree implemented as a map of btnodes for easy traversal
// behaviours are a map to member function pointers, to
// be defined on a derived class.
// BT is thus designed as a pure abstract class, so no
// instances or modifications to bt / btnode are needed...

class bt
{
protected:
	vector<VEC3> pathWpts;
	int currPathWpt;
	int totalPathWpt;

	// the nodes
	virtual map<string, btnode *>* getTree();
	// the C++ functions that implement node actions, hence, the behaviours
	virtual map<string, btaction>* getActions();
	// the C++ functions that implement conditions
	virtual map<string, btcondition>* getConditions();

	virtual btnode** getRoot();
	btnode *current;

	// moved to private as really the derived classes do not need to see this
	btnode *createNode(string);
	btnode *findNode(string);

	void getPath(VEC3 startPoint, VEC3 endPoint, string nombreSala);
	CEntity* frontCollisionIA(VEC3 npcPos, CHandle ownHandle);
	CEntity* frontCollisionBOX(VEC3 npcPos, CEntity *  molePursuingBoxi);
	bool avoidBoxByLeft(CEntity * candidateE, VEC3 npcPos, string nombreSala);
	bool needsSteering(VEC3 npcPos, TCompTransform * transform, float rotation_speed, CHandle myHandle, string nombreSala, CEntity * molePursuingBoxi = nullptr);
public:
	string name;
	// use a derived create to declare BT nodes for your specific BTs
	virtual void create(string);
	// use this two calls to declare the root and the children.
	// use NULL when you don't want a btcondition or btaction (inner nodes)

	btnode *createRoot(string, int, btcondition, btaction); //nombre nodo, tipo nodo, condicion, accion
	btnode *addChild(string, string, int, btcondition, btaction); // nombre padre, nombre hijo, tipo, cond, accion

	// internals used by btnode and other bt calls
	void addAction(string, btaction);
	int execAction(string);
	void addCondition(string, btcondition);
	bool testCondition(string);
	void setCurrent(btnode *);

	// call this once per frame to compute the AI. No need to derive this one,
	// as the behaviours are derived via btactions and the tree is declared on create
	void Recalc();
};

#endif