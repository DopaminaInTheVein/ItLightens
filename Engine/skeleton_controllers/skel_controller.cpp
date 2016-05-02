#include "mcv_platform.h"
#include "skel_controller.h"

#include "components/entity.h"

using namespace std;

void SkelController::init(CHandle new_owner)
{
	owner = new_owner;
}

void SkelController::update()
{
	myUpdate();
	prevState = currentState;
}

void SkelController::setState(string state)
{
	currentState = state;
}

void SkelController::setAnim(string anim, bool loop)
{
	if (!owner.isValid()) return;
	CEntity* eOwner = owner;
	TMsgSetAnim msgAnim;
	msgAnim.name = anim;
	msgAnim.loop = loop;
	eOwner->sendMsg(msgAnim);
}

void SkelController::setLoop(string anim)
{
	setAnim(anim, true);
}

void SkelController::setAction(string anim)
{
	setAnim(anim, false);
}

void SkelController::myUpdate()
{
	if (currentState == prevState) return;
	setLoop(currentState);
}