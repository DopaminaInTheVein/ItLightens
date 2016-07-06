#ifndef INC_LOGIC_PILA_H_
#define	INC_LOGIC_PILA_H_

#include "components/comp_base.h"
#include "handle/handle.h"

#include "components/comp_msgs.h"
#include "logic/aicontroller.h"
class TCompTransform;
class TCompPhysics;
struct TCompPila : public aicontroller, public TCompBase {
	// the states, as maps to functions
	static map<string, statehandler> statemap;
	virtual map<string, statehandler>* getStatemap() { return &statemap; }

	static VHandles all_pilas;

	~TCompPila();
	void onCreate(const TMsgEntityCreated& msg);

	void update(float elapsed);
	TCompPhysics* keepVertical();
	bool load(MKeyValue& atts);
	void setFalling();
	void onContact(const TMsgContact& msg);

	//States
	void OnGround();
	void Grabbed();
	void Falling();
};

#endif