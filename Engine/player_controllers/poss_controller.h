#ifndef _INC_POSS_CONTROLLER_H
#define _INC_POSS_CONTROLLER_H

#include "player_controller_base.h"
#include "components/comp_base.h"
#include "components/entity.h"

#define ST_DISABLED	"disabled"
#define ST_INIT_CONTROL "initControl"

class PossController : public CPlayerBase {
	____TIMER_DECLARE_VALUE_(timerShowEnergy, 1); //Para debugar
	float speedRecover;

public:
	bool npcIsPossessed = false;
	PossController();
	virtual void DisabledState() {};
	virtual void UpdateUnpossess();
	virtual void InitControlState() {};
	virtual CEntity* getMyEntity() = 0; //Pasar al TCompBase?
	void update(float dt) { UpdatePossession(); CPlayerBase::update(dt); }
	void UpdatePossession();
	void onDie(const TMsgDie & msg);
	void onForceUnPosses(const TMsgUnpossesDamage& msg);
	void addPossStates();

	//Mensajes
	void onSetEnable(const TMsgControllerSetEnable&);
	void onSetEnable(bool);

	//FUNCIONES QUE REDEFINIR!
	//virtual void DisabledState();
	//virtual void InitControlState();
	//virtual CEntity* getMyEntity();

	//Load, save and init
	bool load_poss(MKeyValue& atts);
	bool save_poss(std::ofstream& os, MKeyValue& atts);
	void init_poss();
};

#endif