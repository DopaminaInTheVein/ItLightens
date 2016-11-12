#include "mcv_platform.h"
#include "skc_mole.h"

#include "components/comp_charactercontroller.h"
#include "player_controllers/player_controller_mole.h"
#include "components/entity.h"
#include "components/comp_physics.h"
#include "logic/pila.h"

#include "logic/comp_box.h"

//IK Solvers
IK_DECL_SOLVER(grabLeftIK);
IK_DECL_SOLVER(grabRightIK);
IK_DECL_SOLVER(grabPilaIK);
IK_DECL_SOLVER(ungrabbed);
IK_DECL_SOLVER(ungrabbedPila);

void SkelControllerMole::grabObject(CHandle h)
{
	grabbed = h;
	GET_COMP(box, h, TCompBox);
	GET_COMP(tMe, owner, TCompTransform);
	VEC3 pos_grab_dummy;
	box->getGrabPoints(
		tMe
		, left_h_target
		, right_h_target
		, front_h_dir
		, pos_grab_dummy
		, left_h_normal
		, right_h_normal
	);
	enableIK(SK_LHAND, grabLeftIK, SK_MOLE_TIME_TO_GRAB * 0.9f);
	enableIK(SK_RHAND, grabRightIK, SK_MOLE_TIME_TO_GRAB * 0.9f);
}
void SkelControllerMole::grabPila(CHandle h)
{
	grabbedPila = h;
	enableIK(SK_RHAND, grabPilaIK, SK_MOLE_TIME_TO_GRAB * 0.9f);
	GET_COMP(pila, grabbedPila, TCompPila);
	if (pila) pila->Grab();
}
void SkelControllerMole::pushObject(CHandle h) {
	pushed = h;
	GET_COMP(box, h, TCompBox);
	GET_COMP(tMe, owner, TCompTransform);
	VEC3 pos_grab_dummy;
	box->getPushPoints(
		tMe
		, left_h_target
		, right_h_target
		, front_h_dir
		, pos_grab_dummy
		, left_h_normal
		, right_h_normal
	);
	enableIK(SK_LHAND, grabLeftIK, SK_MOLE_TIME_TO_GRAB * 0.9f);
	enableIK(SK_RHAND, grabRightIK, SK_MOLE_TIME_TO_GRAB * 0.9f);
}
void SkelControllerMole::ungrabObject()
{
	GET_COMP(box, grabbed, TCompBox);
	box->UnGrab();
	disableIK(SK_LHAND, SK_MOLE_TIME_TO_UNGRAB, ungrabbed);
	//(SK_RHAND, SK_MOLE_TIME_TO_UNGRAB, );
}
void SkelControllerMole::unpushObject()
{
	GET_COMP(box, pushed, TCompBox);
	box->UnGrab();
	disableIK(SK_LHAND, SK_MOLE_TIME_TO_UNGRAB, ungrabbed);
	disableIK(SK_RHAND, SK_MOLE_TIME_TO_UNGRAB, ungrabbed);
}

void SkelControllerMole::ungrabPila()
{
	GET_COMP(pila, grabbedPila, TCompPila);
	TMsgAttach msg;
	msg.handle = CHandle();
	grabbedPila.sendMsg(msg);
	pila->setFalling();
	grabbedPila = CHandle();
	//disableIK(SK_RHAND, SK_MOLE_TIME_TO_UNGRAB, ungrabbed);
	//(SK_RHAND, SK_MOLE_TIME_TO_UNGRAB, );
}

bool SkelControllerMole::getUpdateInfo()
{
	owner = CHandle(this).getOwner();
	if (!owner.isValid()) return false;
	return true;
}

void SkelControllerMole::SetCharacterController() {
	cc = GETH_COMP(owner, TCompCharacterController);
	assert(cc || fatal("Player doesnt have character controller"));
}

void SkelControllerMole::SetPlayerController() {
	pc = GETH_COMP(owner, player_controller_mole);
	assert(pc || fatal("Player doesnt have player controller"));
}

//Test
#include "skeleton/comp_skeleton.h"
void SkelControllerMole::myUpdate()
{
	updateGrab();
	updatePila();
	updateGrabPoints();
	if (prevState == "idle") {
		int a = 0;
	}
	if (currentState == "idle") {
		int a = 0;
	}
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
		else if (currentState == AST_GRAB_UP) {
			setAction("grab_box_up", "grab_box_idle");
			currentState = AST_GRAB_IDLE;
			disableIK(SK_RHAND);
			//disableIK(SK_LHAND);
			TMsgAttach msgAttach;
			msgAttach.bone_name = SK_RHAND;
			msgAttach.handle = owner;
			msgAttach.save_local_tmx = true;
			msgAttach.offset = VEC3(0, 0, 0);
			grabbed.sendMsg(msgAttach);
		}
		else if (currentState == AST_GRAB_PILA2) {
			setAction("grab_pila_2", "grab_pila_idle");
			currentState = AST_PILA_IDLE;
			disableIK(SK_RHAND);
			//disableIK(SK_LHAND);
			TMsgAttach msgAttach;
			msgAttach.bone_name = SK_RHAND;
			msgAttach.handle = owner;
			msgAttach.save_local_tmx = true;
			grabbedPila.sendMsg(msgAttach);
		}
		else if (currentState == AST_PUT_PILA) {
			//setAction("grab_pila_2", "idle");
			//currentState = AST_IDLE;
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

void SkelControllerMole::updateGrab()
{
	if (grabbed.isValid()) {
		if (currentState == AST_IDLE) currentState = AST_GRAB_IDLE;
		else if (currentState == AST_RUN) currentState = AST_GRAB_WALK;
		else if (currentState == AST_MOVE) currentState = AST_GRAB_WALK;
	}
	else if (pushed.isValid()) {
		if (currentState == AST_IDLE) currentState = AST_GRAB_IDLE;
		else if (currentState == AST_RUN) currentState = AST_PUSH_WALK;
		else if (currentState == AST_MOVE) currentState = AST_PUSH_WALK;
	}
	else {
		if (currentState == AST_GRAB_IDLE) currentState = AST_IDLE;
		else if (currentState == AST_GRAB_WALK) currentState = AST_IDLE;//AST_MOVE;
	}

	if (isMovingBox()) {
		if (grabbed.isValid())
			updateGrabPoints();
		else if (pushed.isValid())
			updatePushPoints();
	}
}

void SkelControllerMole::updatePila()
{
	if (grabbedPila.isValid()) {
		if (currentState == AST_IDLE) currentState = AST_GRAB_IDLE_SMALL;
		else if (currentState == AST_RUN) currentState = AST_GRAB_WALK_SMALL;
		else if (currentState == AST_MOVE) currentState = AST_GRAB_WALK_SMALL;
	}
	else {
		if (currentState == AST_GRAB_IDLE_SMALL) currentState = AST_IDLE;
		else if (currentState == AST_GRAB_WALK_SMALL) currentState = AST_MOVE;
	}
}

void SkelControllerMole::updateGrabPoints()
{
	if (!grabbed.isValid()) return;
	if (isMovingBox()) {
		GET_COMP(box, grabbed, TCompBox);
		GET_MY(tMe, TCompTransform);
		VEC3 pos_grab_dummy;
		box->getGrabPoints(tMe
			, left_h_target
			, right_h_target
			, front_h_dir
			, pos_grab_dummy
			, left_h_normal
			, right_h_normal
			, 0.3f
			, false);
	}
}

void SkelControllerMole::updatePushPoints()
{
	if (!pushed.isValid()) return;
	if (isMovingBox()) {
		GET_COMP(box, pushed, TCompBox);
		GET_MY(tMe, TCompTransform);
		VEC3 pos_grab_dummy;
		box->getPushPoints(tMe
			, left_h_target
			, right_h_target
			, front_h_dir
			, pos_grab_dummy
			, left_h_normal
			, right_h_normal
			, 0.5f
			, false);
	}
}

bool SkelControllerMole::isMovingBox()
{
	return currentState == AST_GRAB_UP
		|| currentState == AST_GRAB_IDLE
		|| currentState == AST_GRAB_WALK
		|| currentState == AST_PUSH_IDLE
		|| currentState == AST_PUSH_PREP
		|| currentState == AST_PUSH_WALK
		|| currentState == AST_PULL_WALK
		;
}

VEC3 SkelControllerMole::getGrabLeft()
{
	return left_h_target;
}

VEC3 SkelControllerMole::getGrabRight()
{
	return right_h_target;
}
VEC3 SkelControllerMole::getGrabNormalLeft()
{
	return left_h_normal;
}
VEC3 SkelControllerMole::getGrabNormalRight()
{
	return right_h_normal;
}
VEC3 SkelControllerMole::getGrabFrontDir()
{
	return front_h_dir;
}

CHandle SkelControllerMole::getGrabbed()
{
	if (grabbed.isValid())
		return grabbed;
	else
		return pushed;
}

CHandle SkelControllerMole::getGrabbedPila()
{
	return grabbedPila;
}

IK_IMPL_SOLVER(grabLeftIK, info, result) {
	GET_COMP(skc, info.handle, SkelControllerMole);
	GET_COMP(tmx, skc->getGrabbed(), TCompTransform);
	GET_COMP(tmx_mole, info.handle, TCompTransform);
	result.new_pos = skc->getGrabLeft();
	//result.bone_normal = VEC3(0, 1, 0);
	//result.bone_normal = result.new_pos - tmx->getPosition();
	//result.bone_normal = tmx_mole->getLeft();
	result.bone_normal = skc->getGrabNormalRight();
	result.bone_normal.Normalize();
	Debug->DrawLine(VEC3(0, 100, 0), result.new_pos);
}

IK_IMPL_SOLVER(grabRightIK, info, result) {
	GET_COMP(skc, info.handle, SkelControllerMole);
	result.new_pos = skc->getGrabRight();
	result.bone_normal = skc->getGrabNormalLeft();
}

IK_IMPL_SOLVER(ungrabbed, info, result) {
	GET_COMP(skc, info.handle, SkelControllerMole);
	TMsgAttach msg;
	msg.handle = CHandle();
	skc->getGrabbed().sendMsg(msg);
	skc->removeGrab();
}

IK_IMPL_SOLVER(grabPilaIK, info, result) {
	GET_COMP(mole_t, info.handle, TCompTransform);
	GET_COMP(skc, info.handle, SkelControllerMole);
	GET_COMP(pila_t, skc->getGrabbedPila(), TCompTransform);
	VEC3 right = -mole_t->getLeft();
	right.Normalize();
	result.new_pos = pila_t->getPosition() + right * 0.3f;

	//result.bone_normal = skc->getGrabNormalLeft();
}

IK_IMPL_SOLVER(ungrabbedPila, info, result) {
	GET_COMP(skc, info.handle, SkelControllerMole);
	TMsgAttach msg;
	msg.handle = CHandle();
	skc->getGrabbedPila().sendMsg(msg);
	skc->removePila();
}