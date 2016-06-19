#include "mcv_platform.h"
#include "skc_mole.h"

#include "components/comp_charactercontroller.h"
#include "player_controllers/player_controller_mole.h"
#include "components/entity.h"


bool SkelControllerMole::getUpdateInfo()
{
	owner = CHandle(this).getOwner();
	if (!owner.isValid()) return false;
	return true;
}

void SkelControllerMole::SetCharacterController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		cc = eMe->get<TCompCharacterController>();
		assert(cc || fatal("Player doesnt have character controller"));
	}
}

void SkelControllerMole::SetPlayerController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		pc = eMe->get<player_controller_mole>();
		assert(pc || fatal("Player doesnt have player controller"));
	}
}

void SkelControllerMole::myUpdate()
{
	if (currentState == "walk" || currentState == "run") {
		SetPlayerController();
		if (!pc->isMoving()) {
			currentState = "idle";
		}
		////TODO: read moving param!
		//VEC3 speed = cc->GetSpeed();
		//VEC3 lastSpeed = cc->GetLastSpeed();
		//speed.y = 0; // No tenemos en cuenta la velocidad vertical!
		//lastSpeed.y = 0; // No tenemos en cuenta la velocidad vertical!
		//dbg("Player Speed, Last: %f, %f\n", speed.LengthSquared(), lastSpeed.LengthSquared());

		//if ( !isNormal(speed) || speed.LengthSquared() <= 0.0001f) {
		//	
		//}
	}

	if (currentState != prevState) {
		if (currentState == "jump") {
			setAction("jump", "jumpidle");
		}
		else if (currentState == "idle" && prevState == "jumpland") {
			setAction("jumplandidle", "idle");
		}
		else if (currentState == "attack") {
			setAction("attack", "idle");
		}
		else if (currentState == "recharge") {
			setAction("recharge", "idle");
			currentState = "idle";
		}
		else {
			setLoop(currentState);
		}
	}

	priority = false;
}