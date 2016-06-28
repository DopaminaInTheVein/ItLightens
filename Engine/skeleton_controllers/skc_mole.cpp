#include "mcv_platform.h"
#include "skc_mole.h"

#include "components/comp_charactercontroller.h"
#include "player_controllers/player_controller_mole.h"
#include "components/entity.h"

#include "logic/comp_box.h"

//IK Solvers
IK_DECL_SOLVER(grabLeftIK);
IK_DECL_SOLVER(grabRightIK);

void SkelControllerMole::grabObject(CHandle h)
{
	grabbed = h;
	GET_COMP(box, h, TCompBox);
	GET_COMP(tMe, owner, TCompTransform);
	box->getGrabPoints(tMe->getPosition(), left_h_target, right_h_target, front_h_dir);
	enableIK(SK_LHAND, grabLeftIK, SK_MOLE_TIME_TO_GRAB);
	enableIK(SK_RHAND, grabRightIK, SK_MOLE_TIME_TO_GRAB);
}

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
		else if (currentState == AST_GRAB_1) {
			setAction("grab_box", "idle");
			currentState = "idle";
		}
		else if (currentState == AST_GRAB_IDLE) {
			//setAction("grab_box", "grab_box");
			currentState = AST_GRAB_IDLE;
			disableIK(SK_RHAND);
			disableIK(SK_LHAND);
			TMsgAttach msgAttach;
			msgAttach.bone_name = SK_LHAND;
			msgAttach.handle = owner;
			msgAttach.save_local_tmx = true;
			grabbed.sendMsg(msgAttach);
		}
		else {
			setLoop(currentState);
		}
	}

	priority = false;
}

VEC3 SkelControllerMole::getGrabLeft()
{
	return left_h_target;
}

VEC3 SkelControllerMole::getGrabRight()
{
	return right_h_target;
}

VEC3 SkelControllerMole::getGrabFrontDir()
{
	return front_h_dir;
}

CHandle SkelControllerMole::getGrabbed()
{
	return grabbed;
}

IK_IMPL_SOLVER(grabLeftIK, info, result) {
	GET_COMP(skc, info.handle, SkelControllerMole);
	GET_COMP(tmx, skc->getGrabbed(), TCompTransform);
	//result.bone_front = skc->getGrabFrontDir();//tmx->getPosition();
	result.offset_pos = skc->getGrabLeft() - info.bone_pos;
}

IK_IMPL_SOLVER(grabRightIK, info, result) {
	GET_COMP(skc, info.handle, SkelControllerMole);
	GET_COMP(tmx, skc->getGrabbed(), TCompTransform);
	//result.bone_front = skc->getGrabFrontDir();//tmx->getPosition();
	result.offset_pos = skc->getGrabRight() - info.bone_pos;
}