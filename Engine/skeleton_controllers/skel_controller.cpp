#include "mcv_platform.h"
#include "skel_controller.h"
#include "skc_guard.h"
#include "skc_mole.h"
#include "skc_player.h"
#include "skc_scientist.h"

#include "components/entity.h"
#include "skeleton/comp_skeleton.h"

#include "app_modules/logic_manager/logic_manager.h"
using namespace std;

IK_DECL_SOLVER(lfootIK);
IK_DECL_SOLVER(rfootIK);

void TCompSkelController::onCreate(const TMsgEntityCreated&)
{
	getUpdateInfo();
	GET_COMP(sk, owner, TCompSkeleton);
	if (sk) {
		lfoot_height_offset = sk->getLFootOffset();
		rfoot_height_offset = sk->getRFootOffset();
		enableIK("LFoot", lfootIK, 0.1f);
		enableIK("RFoot", rfootIK, 0.1f);
	}
}

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
void TCompSkelController::disableIK(std::string bone_name, float delay, IK::bone_solver function)
{
	TMsgSetIKSolver msgIK;
	msgIK.enable = false;
	msgIK.bone_name = bone_name;
	msgIK.handle = owner;
	msgIK.function = function;
	msgIK.time = delay;
	owner.sendMsg(msgIK);
}

void TCompSkelController::renderInMenuParent()
{
	ImGui::Text("STATE: %s", currentState.c_str());
	ImGui::DragFloat("LFoot offset", &lfoot_height_offset, 0.01f, -.5f, .5f);
	ImGui::DragFloat("RFoot offset", &rfoot_height_offset, 0.01f, -.5f, .5f);
	ImGui::DragFloat("LFoot correction", &lfoot_correction, 0.01f, -.5f, .5f);
	ImGui::DragFloat("RFoot correction", &rfoot_correction, 0.01f, -.5f, .5f);
	IMGUI_SHOW_BOOL(foot_ground_left);
	IMGUI_SHOW_BOOL(foot_ground_right);
}
void TCompSkelController::solveFootLeft(const IK::InfoSolver& info, IK::ResultSolver& result)
{
	solveFoot(info, result, lfoot_correction, foot_ground_left, lfoot_height_offset);
}
void TCompSkelController::solveFootRight(const IK::InfoSolver& info, IK::ResultSolver& result)
{
	solveFoot(info, result, rfoot_correction, foot_ground_right, rfoot_height_offset);
}
void TCompSkelController::solveFoot(
	const IK::InfoSolver& info
	, IK::ResultSolver& result
	, float& height_correction
	, bool& on_ground
	, float height_offset)
{
	GET_COMP(sk, owner, TCompSkeleton);
	//Raycast
	PxQueryFilterData filterData;
	filterData.data.word0 = ItLightensFilter::eSCENE | ItLightensFilter::eOBJECT;
	PxRaycastBuffer hit;
	VEC3 orig = info.bone_pos + VEC3_UP * 0.1f;
	VEC3 end = info.bone_pos - VEC3_UP * (0.1f + height_offset);
	bool hit_ground = g_PhysxManager->raycast(orig, end, hit, filterData);
	float dist = 1.f;
	Debug->DrawLine(orig, end);
	if (hit_ground) {
		dist = hit.getAnyHit(0).distance - 0.1f - height_offset;
		if (dist > 0) {
			height_correction = dist;
		}
		else {
			height_correction = 0;
		}
	}
	result.new_pos = info.bone_pos + VEC3(0.f, height_correction, 0.f);
	GET_COMP(tmx, info.handle, TCompTransform);
	result.bone_normal = -tmx->getLeft();
	result.height_fix = height_correction;

	updateSteps(abs(dist), on_ground);
}
void TCompSkelController::updateSteps(float dist_ground, bool& on_ground)
{
	bool send_msg = false;
	//dbg("Dist ground: %f, on_ground: %d", dist_ground, on_ground);
	if (on_ground) {
		if (dist_ground > FOOT_AIR_EPSILON) {
			send_msg = true;
			on_ground = false;
		}
	}
	else {
		if (dist_ground < FOOT_GROUND_EPSILON) {
			send_msg = true;
			//logic_manager...
			on_ground = true;
		}
	}
	//dbg("-->new_on_ground: %d\n", on_ground);
	if (send_msg) {
		TMsgGetWhoAmI msg;
		owner.sendMsgWithReply(msg);
		if (on_ground) logic_manager->throwEvent(CLogicManagerModule::EVENT::OnStep, msg.who_string, owner);
		else logic_manager->throwEvent(CLogicManagerModule::EVENT::OnStepOut, msg.who_string, owner);
	}
}

IK_IMPL_SOLVER(lfootIK, info, result) {
	TCompSkelController* skc = nullptr;
	GET_COMP(skcGuard, info.handle, SkelControllerGuard);
	GET_COMP(skcMole, info.handle, SkelControllerMole);
	GET_COMP(skcPlayer, info.handle, SkelControllerPlayer);
	GET_COMP(skcSci, info.handle, SkelControllerScientist);
	if (skcGuard) skc = skcGuard;
	else if (skcMole) skc = skcMole;
	else if (skcPlayer) skc = skcPlayer;
	else if (skcSci) skc = skcSci;
	else return;
	skc->solveFootLeft(info, result);
}

IK_IMPL_SOLVER(rfootIK, info, result) {
	TCompSkelController* skc = nullptr;
	GET_COMP(skcGuard, info.handle, SkelControllerGuard);
	GET_COMP(skcMole, info.handle, SkelControllerMole);
	GET_COMP(skcPlayer, info.handle, SkelControllerPlayer);
	GET_COMP(skcSci, info.handle, SkelControllerScientist);
	if (skcGuard) skc = skcGuard;
	else if (skcMole) skc = skcMole;
	else if (skcPlayer) skc = skcPlayer;
	else if (skcSci) skc = skcSci;
	else return;
	skc->solveFootRight(info, result);
}