#ifndef _INC_POSS_CONTROLLER_H
#define _INC_POSS_CONTROLLER_H

#include "player_controller_base.h"
#include "components/comp_base.h"
#include "components/entity.h"
#include "logic/ai_poss.h"

#define ST_DISABLED	"disabled"
#define ST_INIT_CONTROL "initControl"

class PossController : public CPlayerBase {
	____TIMER_DECLARE_(timerShowEnergy, 1); //Para debugar
	float speedRecover;

public:
	bool npcIsPossessed = false;
	PossController();
	virtual void DisabledState() {};
	virtual void InitControlState() {};
	virtual CEntity* getMyEntity() = 0; //Pasar al TCompBase?
	void update(float dt) { UpdatePossession(); CPlayerBase::update(dt); }
	void UpdatePossession();

	//Mensajes
	void onSetEnable(const TMsgControllerSetEnable&);
	void onSetEnable(bool);

	//FUNCIONES QUE REDEFINIR!
	//virtual void DisabledState();
	//virtual void InitControlState();
	//virtual CEntity* getMyEntity();
};

#endif