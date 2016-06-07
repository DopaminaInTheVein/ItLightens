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

void SkelController::setState(string state, bool prio)
{
	if (!priority) {
		priority = prio;
		currentState = state;
	}
}

void SkelController::setAnim(string anim, bool loop, string nextLoop)
{
	std::vector<std::string> anims;
	anims.push_back(anim);

	std::vector<std::string> next;
	next.push_back(nextLoop);

	setAnim(anims, loop, next);
}

void SkelController::setLoop(string anim)
{
	std::vector<std::string> anims;
	anims.push_back(anim);

	std::vector<std::string> next;
	next.push_back("");

	setAnim(anims, true, next);
}

void SkelController::setAction(string anim, string next_loop)
{
	std::vector<std::string> anims;
	anims.push_back(anim);

	std::vector<std::string> next;
	next.push_back(next_loop);

	setAnim(anims, false, next);
}

//animaciones parciales
void SkelController::setAnim(std::vector<std::string> anim, bool loop, std::vector<std::string> nextLoop) {

	if (!owner.isValid()) return;
	CEntity* eOwner = owner;
	TMsgSetAnim msgAnim;
	msgAnim.name = anim;
	msgAnim.loop = loop;
	msgAnim.nextLoop = nextLoop;
	eOwner->sendMsg(msgAnim);

}

void SkelController::setAction(std::vector<std::string> anim, std::vector<std::string> nextLoop)  {

	setAnim(anim, false, nextLoop);

}

void SkelController::setLoop(std::vector<std::string> anim) {

	std::vector<std::string> next;
	next.push_back("");

	setAnim(anim, true, next);

}

void SkelController::myUpdate()
{
	if (currentState == prevState) return;
	setLoop(currentState);
}