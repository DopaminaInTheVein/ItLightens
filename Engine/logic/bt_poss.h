#ifndef _BT_POSS_INC
#define _BT_POSS_INC

#include "mcv_platform.h"
#include "bt.h"
#include "components/comp_msgs.h"
#include "components/entity.h"
#include <string>

#define ST_POSSESSING	"possessionStart"
#define ST_POSSESSED	"possessed"
#define ST_UNPOSSESSING	"possessionEnd"

#define ST_STUNT			"stunt"
#define ST_STUNT_END		"stuntEnd"

class bt_poss : public bt {
protected:
	bool stunned = false;
	bool possessed = false;
	bool stunning = false;
	bool possessing = false;

	// the nodes
	virtual map<string, btnode *>* getTree();
	// the C++ functions that implement node actions, hence, the behaviours
	virtual map<string, btaction>* getActions();
	// the C++ functions that implement conditions
	virtual map<string, btcondition>* getConditions();
	// the events that will be executed by the decoratos
	virtual map<string, btevent>* getEvents();

	virtual btnode** getRoot();

public:
	bt_poss();
	void addBtPossStates();

	//conditions
	bool npcAvailable();
	bool beingPossessed();
	bool beingUnpossessed();
	//actions
	int actionPossessing();
	int actionPossessed();
	int actionUnpossessing();
	int actionStunt();
	int actionStuntEnd();

	____TIMER_DECLARE_VALUE_(timeStunt, 10.0f);
	//Preguntar por Stunt o Poseido
	bool isStunned() { return stunned; }
	bool isPossessed() { return possessed; }
	bool isAvailable() { return !stunned && !possessed; }

	//Accion que se ejecuta antes de empezar el proceso de posesión
	//(útil para guardarse último estado antes de ser poseído, etc.)
	virtual void _actionBeforePossession();
	virtual void _actionBeingUnpossessed();
	virtual void _actionWhenStunt();

	//Avisar a la entidad que ha sido poseída
	void onSetPossessed(const TMsgAISetPossessed&);
	//Avisar a la entidad que ha sido stunneada
	void onSetStunned(const TMsgAISetStunned&);

	virtual void onStaticBomb(const TMsgStaticBomb & msg);
	//TComponent
	virtual CEntity* getMyEntity() = 0; //Pasar al TCompBase?
	};

#endif