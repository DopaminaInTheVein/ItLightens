#include "mcv_platform.h"
#include "skc_player.h"

#include "components/comp_charactercontroller.h"
#include "player_controllers/player_controller.h"
#include "components/entity.h"
#include "components/comp_controller_3rd_person.h"

bool SkelControllerPlayer::getUpdateInfo()
{
	owner = CHandle(this).getOwner();
	if (!owner.isValid()) return false;
	return true;
}

void SkelControllerPlayer::SetCharacterController() {
	cc = GETH_COMP(owner, TCompCharacterController);
	assert(cc || fatal("Player doesnt have character controller"));
}

void SkelControllerPlayer::SetPlayerController() {
	pc = GETH_COMP(owner, player_controller);
	assert(pc || fatal("Player doesnt have player controller"));
}

void SkelControllerPlayer::myUpdate()
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
		else if (currentState == AST_DEATH) {
			setAction(AST_DEATH, AST_NULL);
		}
		else {
			setLoop(currentState);
		}
	}

	priority = false;
}

void SkelControllerPlayer::myUpdateIK()
{
}