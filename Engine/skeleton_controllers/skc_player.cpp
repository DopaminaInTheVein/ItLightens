#include "mcv_platform.h"
#include "skc_player.h"

#include "components/comp_charactercontroller.h"
#include "player_controllers/player_controller.h"
#include "components/entity.h"
#include "skeleton/skeleton_manager.h"

//IK Solvers
IK_DECL_SOLVER(heilTest);

bool SkelControllerPlayer::getUpdateInfo()
{
	owner = CHandle(this).getOwner();
	if (!owner.isValid()) return false;
	return true;
}

void SkelControllerPlayer::SetCharacterController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		cc = eMe->get<TCompCharacterController>();
		assert(cc || fatal("Player doesnt have character controller"));
	}
}

void SkelControllerPlayer::SetPlayerController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		pc = eMe->get<player_controller>();
		assert(pc || fatal("Player doesnt have player controller"));
	}
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
		else {
			setLoop(currentState);
		}
	}

	priority = false;
}

void SkelControllerPlayer::myUpdateIK()
{
	if (currentState != prevState) {
		if (currentState == "jump") {
			TMsgSetIKSolver msgIK;
			msgIK.enable = true;
			msgIK.bone_name = SK_RHAND;
			msgIK.handle = CHandle(this).getOwner();
			msgIK.function = &heilTest;
			msgIK.time = 0.5f;
			compBaseEntity->sendMsg(msgIK);
		}
		//else if ( currentState == "" ) {

		//}
		//else {

		//}

		if (prevState == "jump") {
			TMsgSetIKSolver msgIK;
			msgIK.enable = false;
			msgIK.bone_name = SK_RHAND;
			msgIK.handle = CHandle(this).getOwner();
			msgIK.function = &heilTest;
			compBaseEntity->sendMsg(msgIK);
		}
	}

}

IK_IMPL_SOLVER(heilTest, info, result) {
	result.offset_pos = VEC3(0.f, 0.3f, 0.f);
}