#ifndef INC_LOGIC_PILA_H_
#define	INC_LOGIC_PILA_H_

#include "components/comp_base.h"
#include "handle/handle.h"

#include "components/comp_msgs.h"
#include "components/comp_trigger_lua.h"
#include "logic/aicontroller.h"

class TCompTransform;
class TCompPhysics;
struct TCompPila : public aicontroller, public TTriggerLua {
	// the states, as maps to functions
	static map<string, statehandler> statemap;
	virtual map<string, statehandler>* getStatemap() { return &statemap; }

	static VHandles all_pilas;
	CHandle player;
	VEC3 place;
	CHandle container;
	bool charged;

	~TCompPila();
	void onCreate(const TMsgEntityCreated& msg);
	void init();

	void update(float elapsed);
	TCompPhysics* keepVertical();
	bool load(MKeyValue& atts);
	void setFalling();
	void onContact(const TMsgContact& msg);
	void checkActions();
	bool getPlayer();
	bool isPlayerNear();
	eAction getActionAvailable() override;
	void executeTrigger(CLogicManagerModule::EVENT logicEvent) override;
	void OnGround();
	void Grabbed();
	void StartFalling();
	void Falling();
	void PutIn(CHandle, bool notify = true);
	void Grab();
	void onRecharge(const TMsgActivate&);
	void isCharged(TMsgIsCharged&);
	bool isCharged() const { return charged; };
};

#endif