#include "mcv_platform.h"
#include "skel_controller.h"

#include "components/entity.h"
#include "components/comp_transform.h"
#include "skeleton/comp_skeleton.h"

using namespace std;

void SkelController::init(CHandle new_owner)
{
	owner = new_owner;
	logicYaw = getYaw();
}

void SkelController::update()
{
	myUpdate();

	// yaw update during animation	
	if (currentState == AST_TURNL || currentState == AST_TURNR) {
		//dbg("Anim Frame Duration: %f", getAnimFrameDuration(currentState));
		float delta_yaw = (deg2rad(9.f) * getDeltaTime()) / getAnimFrameDuration(currentState);
		if (currentState != prevState) {
			logicYaw = getYaw();
		}
		else {
			if (currentState == AST_TURNL)
				delta_yaw = -delta_yaw;
			logicYaw += delta_yaw; //TODO: getDeltaYaw from anim?
		}
	}

	// if the rotation finishes, update the logic yaw
	if ((prevState == AST_TURNL || prevState == AST_TURNR) &&
		currentState != prevState) {
		setYaw();
	}

	prevState = currentState;
}

void SkelController::setState(string state)
{
	currentState = state;
}

void SkelController::setAnim(string anim, bool loop, string nextLoop, float blendTime)
{
	if (!owner.isValid()) return;
	CEntity* eOwner = owner;
	TMsgSetAnim msgAnim;
	msgAnim.name = anim;
	msgAnim.loop = loop;
	msgAnim.nextLoop = nextLoop;
	msgAnim.blendTime = blendTime;
	eOwner->sendMsg(msgAnim);
}

void SkelController::setLoop(string anim)
{
	if ((currentState == AST_TURNL || currentState == AST_TURNR) && currentState != prevState) {
		setAnim(anim, true, "", 0.f);
	}
	else if ((prevState == AST_TURNL || prevState == AST_TURNR) && currentState != prevState) {
		setAnim(anim, true, "", 0.f);
	}
	else {
		setAnim(anim, true);
	}
}

void SkelController::setAction(string anim, string next_loop)
{
	setAnim(anim, false, next_loop);
}

void SkelController::myUpdate()
{
	if (currentState == prevState) return;
	setLoop(currentState);
}

float SkelController::getLogicYaw() {
	return logicYaw;
}

float SkelController::getYaw()
{
	float yaw = 0.0f;
	float pitch = 0.0f;
	if (owner.isValid()) {
		CEntity* eMe = owner;
		TCompTransform * t = eMe->get<TCompTransform>();
		assert(t || fatal("Player doesnt have transform"));
		t->getAngles(&yaw, &pitch);
	}
	return yaw;
}

void SkelController::setYaw()
{
	float yaw = 0.0f;
	float pitch = 0.0f;
	if (owner.isValid()) {
		CEntity* eMe = owner;
		TCompTransform * t = eMe->get<TCompTransform>();
		assert(t || fatal("Player doesnt have transform"));
		t->getAngles(&yaw, &pitch);
		t->setAngles(logicYaw, pitch);
	}
}

float SkelController::getAnimFrameDuration(string anim_name)
{
	if (owner.isValid()) {
		CEntity* eMe = owner;
		TCompSkeleton * sk = eMe->get<TCompSkeleton>();
		assert(sk || fatal("Player doesnt have skeleton"));
		return sk->getFrameDuration(anim_name);
	}
	
	return -1.f;
}