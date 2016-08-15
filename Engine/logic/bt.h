#ifndef _BT_INC
#define _BT_INC

#include <string>
#include <map>
#include "btnode.h"
#include "components/components.h"
#include "app_modules\logic_manager\logic_manager.h"

using namespace std;

typedef int (bt::*btaction)();
typedef bool (bt::*btcondition)();

struct btevent {
	CLogicManagerModule::EVENT evt;
	string params;
};

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
	// the events that will be executed by the decoratos
	virtual map<string, btevent>* getEvents();

	virtual btnode** getRoot();
	btnode *current;

	std::string state_ini;

	// moved to private as really the derived classes do not need to see this
	btnode *createNode(string);
	btnode *findNode(string);

	bool getPath(const VEC3& startPoint, const VEC3& endPoint);
	CEntity* frontCollisionIA(const VEC3 & npcPos, ClHandle ownHandle);
	CEntity* frontCollisionBOX(const TCompTransform * transform, CEntity *  molePursuingBoxi);
	bool avoidBoxByLeft(CEntity * candidateE, const TCompTransform * transform);
	bool needsSteering(VEC3 npcPos, TCompTransform * transform, float rotation_speed, ClHandle myHandle, CEntity * molePursuingBoxi = nullptr);
	virtual bool load_bt(MKeyValue& atts);
	virtual bool save_bt(std::ofstream& os, MKeyValue& atts);

public:
	//Prueba
	int getPathDebug(const VEC3& startPoint, const VEC3& endPoint) {
		if (getPath(startPoint, endPoint)) return totalPathWpt;
		else return -1;
	}

	string name;
	// use a derived create to declare BT nodes for your specific BTs
	virtual void create(string);
	// use this two calls to declare the root and the children.
	// use NULL when you don't want a btcondition or btaction (inner nodes)

	btnode *createRoot(string, int, btcondition, btaction); //nombre nodo, tipo nodo, condicion, accion
	btnode *addChild(string, string, int, btcondition, btaction); // nombre padre, nombre hijo, tipo, cond, accion
	btnode *addChild(string, string, int, btcondition, CLogicManagerModule::EVENT, string); // nombre padre, nombre hijo, tipo, cond, evento, parametros

	// internals used by btnode and other bt calls
	void addAction(string, btaction);
	int execAction(string);
	void addCondition(string, btcondition);
	bool testCondition(string);
	void addEvent(string, btevent);
	int execEvent(string);
	void setCurrent(btnode *);

	void initParent() {
		currPathWpt = totalPathWpt = 0;
		pathWpts.clear();
		if (state_ini != "") setCurrent(findNode(state_ini));
		else setCurrent(NULL);
	}

	// call this once per frame to compute the AI. No need to derive this one,
	// as the behaviours are derived via btactions and the tree is declared on create
	void Recalc();
};

#endif