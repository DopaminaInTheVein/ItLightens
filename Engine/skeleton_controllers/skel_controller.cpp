#include "mcv_platform.h"
#include "skel_controller.h"

#include "components/entity.h"

using namespace std;

void TCompSkelController::update()
{
	myUpdate();
	myUpdateIK();
	prevState = currentState;
}

void TCompSkelController::setState(string state, bool prio)
{
	if (!priority) {
		priority = prio;
		currentState = state;
	}
}

void TCompSkelController::setAnim(string anim, bool loop, string nextLoop)
{
	std::vector<std::string> anims;
	anims.push_back(anim);

	std::vector<std::string> next;
	next.push_back(nextLoop);

	setAnim(anims, loop, next);
}

void TCompSkelController::setLoop(string anim)
{
	std::vector<std::string> anims;
	anims.push_back(anim);

	std::vector<std::string> next;
	next.push_back("");

	setAnim(anims, true, next);
}

void TCompSkelController::setAction(string anim, string next_loop)
{
	std::vector<std::string> anims;
	anims.push_back(anim);

	std::vector<std::string> next;
	next.push_back(next_loop);

	setAnim(anims, false, next);
}

//animaciones parciales
void TCompSkelController::setAnim(std::vector<std::string> anim, bool loop, std::vector<std::string> nextLoop)
{
	if (!owner.isValid()) return;
	CEntity* eOwner = owner;
	TMsgSetAnim msgAnim;
	msgAnim.name = anim;
	msgAnim.loop = loop;
	msgAnim.nextLoop = nextLoop;
	eOwner->sendMsg(msgAnim);
}

void TCompSkelController::setAction(std::vector<std::string> anim, std::vector<std::string> nextLoop)
{
	setAnim(anim, false, nextLoop);
}

void TCompSkelController::setLoop(std::vector<std::string> anim)
{
	std::vector<std::string> next;
	next.push_back("");
	setAnim(anim, true, next);
}

void TCompSkelController::myUpdate()
{
	if (currentState == prevState) return;
	setLoop(currentState);
}

// IK Methods
void TCompSkelController::myUpdateIK() {}
void TCompSkelController::enableIK(std::string bone_name, IK::bone_solver solver, float delay)
{
	TMsgSetIKSolver msgIK;
	msgIK.enable = true;
	msgIK.bone_name = bone_name;
	msgIK.handle = owner;
	msgIK.function = solver;
	msgIK.time = delay;
	owner.sendMsg(msgIK);
}
void TCompSkelController::disableIK(std::string bone_name)
{
	TMsgSetIKSolver msgIK;
	msgIK.enable = false;
	msgIK.bone_name = bone_name;
	msgIK.handle = owner;
	owner.sendMsg(msgIK);
}

void TCompSkelController::renderInMenu()
{
	ImGui::Text("STATE: %s", currentState.c_str());
}