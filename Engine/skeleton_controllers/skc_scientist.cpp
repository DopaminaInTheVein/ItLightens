#include "mcv_platform.h"
#include "skc_scientist.h"

#include "components/comp_charactercontroller.h"
#include "player_controllers/player_controller_cientifico.h"
#include "components/entity.h"

bool SkelControllerScientist::getUpdateInfo()
{
	owner = ClHandle(this).getOwner();
	if (!owner.isValid()) return false;
	return true;
}

void SkelControllerScientist::SetCharacterController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		cc = eMe->get<TCompCharacterController>();
		assert(cc || fatal("Player doesnt have character controller"));
	}
}

void SkelControllerScientist::SetPlayerController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		pc = eMe->get<player_controller_cientifico>();
		assert(pc || fatal("Player doesnt have player controller"));
	}
}

void SkelControllerScientist::myUpdate()
{
	if (currentState == "walk" || currentState == "run") {
		SetPlayerController();
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
		else if (currentState == "throw") {
			setAction("throw", "idle");
		}
		else if (currentState == AST_FALL) {
			setAction("jumpland", "idle");
			currentState = AST_IDLE;
		}
		else {
			setLoop(currentState);
		}
	}

	priority = false;
}