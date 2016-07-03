#include "mcv_platform.h"
#include "player_controller_mole.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"
#include "app_modules\io\io.h"
#include "app_modules\logic_manager\logic_manager.h"
#include "components\comp_msgs.h"

#include "ui\ui_interface.h"
#include "components/comp_charactercontroller.h"
#include "components\comp_physics.h"

map<string, statehandler> player_controller_mole::statemap = {};

//if (animController) animController->setState(AST_IDLE, [prio])
#define SET_ANIM_MOLE(state) SET_ANIM_STATE(animController, state)
#define SET_ANIM_MOLE_P(state) SET_ANIM_STATE_P(animController, state)

//State names
#define ST_MOLE_GRAB "grabbedBox"
#define ST_MOLE_GRAB_GO "goToGrab"
#define ST_MOLE_GRAB_FACE "faceToGrab"
#define ST_MOLE_GRABBING_1 "grabbing1"
#define ST_MOLE_GRABBING_2 "grabbing2"
#define ST_MOLE_GRABBING_IMPACT "grabbing_impact"
#define ST_MOLE_GRABBING_IMPACT_1 "grabbing_impact_1" //Lose box control
#define ST_MOLE_GRABBING_IMPACT_2 "grabbing_impact_2" //Recovery box control
#define ST_MOLE_UNGRAB "leaveBox"
#define ST_MOLE_UNGRABBING "leavingBox"
#define ST_MOLE_DESTROY "destroyWall"

#define MOLE_TIME_OUT_GO_GRAB	4.f

void player_controller_mole::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("AI_mole")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields_base = readIniAtrData(file_ini, "controller_base");

			assignValueToVar(player_max_speed, fields_base);
			assignValueToVar(player_rotation_speed, fields_base);
			assignValueToVar(jimpulse, fields_base);
			assignValueToVar(left_stick_sensibility, fields_base);
			assignValueToVar(camera_max_height, fields_base);
			assignValueToVar(camera_min_height, fields_base);

			map<std::string, float> fields_mole = readIniAtrData(file_ini, "controller_mole");

			assignValueToVar(grab_box_energy, fields_mole);
			assignValueToVar(destroy_wall_energy, fields_mole);
		}
	}
}

#define AddStMole(name, func) AddState(name, (statehandler)&player_controller_mole::func)
void player_controller_mole::Init() {
	getUpdateInfoBase(CHandle(this).getOwner());

	// read main attributes from file
	readIniFileAttr();
	mole_max_speed = player_max_speed;
	____TIMER_REDEFINE_(t_grab_hit, 0.1f);
	____TIMER__SET_ZERO_(t_grab_hit);

	om = getHandleManager<player_controller_mole>();	//player

	if (statemap.empty()) {
		//States from controller base and poss controller
		addBasicStates();
		addPossStates();
		AddStMole(ST_MOLE_GRAB_GO, GoToGrab);
		AddStMole(ST_MOLE_GRAB_FACE, FaceToGrab);
		AddStMole(ST_MOLE_GRABBING_1, GrabbingBox1);
		AddStMole(ST_MOLE_GRABBING_2, GrabbingBox2);
		AddStMole(ST_MOLE_GRABBING_IMPACT, GrabbingImpact);
		AddStMole(ST_MOLE_GRABBING_IMPACT_1, GrabbingImpact1);
		AddStMole(ST_MOLE_GRABBING_IMPACT_2, GrabbingImpact2);
		AddStMole(ST_MOLE_GRAB, GrabbedBox);
		AddStMole(ST_MOLE_UNGRAB, LeaveBox);
		AddStMole(ST_MOLE_UNGRABBING, LeavingBox);
		AddStMole(ST_MOLE_DESTROY, DestroyWall);
	}

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
	SET_ANIM_MOLE(AST_IDLE);
}

bool player_controller_mole::getUpdateInfo()
{
	if (!CPlayerBase::getUpdateInfo()) return false;
	myParent = CHandle(this).getOwner();
	animController = GETH_MY(SkelControllerMole);
	return true;
}

void player_controller_mole::myUpdate()
{
	//dbg("mole frame\n");

	if (this->nearToBox()) {
		VEC3 left, right, front_dir, pos_grab;
		GET_COMP(box, boxNear, TCompBox);
		box->getGrabPoints(transform, left, right, front_dir, pos_grab);
		Debug->DrawLine(pos_grab, left, VEC3(1, 1, 0));
		Debug->DrawLine(pos_grab, right, VEC3(0, 0, 1));
	}
	Debug->DrawLine(transform->getPosition(), transform->getFront(), 1.f);
}

void player_controller_mole::UpdateInputActions() {
	if (io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed()) {
		if (boxGrabbed.isValid()) {
			logic_manager->throwEvent(logic_manager->OnLeaveBox, "");
			ChangeState(ST_MOLE_UNGRAB);
		}
		else {
			if (this->nearToBox()) {
				//ChangePose(pose_box_route);
				GET_COMP(box, boxNear, TCompBox);
				VEC3 h_target_dummy;
				box->getGrabPoints(transform, h_target_dummy, h_target_dummy, grabInfo.dir_to_grab, grabInfo.pos_to_grab);
				float pitch_dummy;
				getYawPitchFromVector(grabInfo.dir_to_grab, &grabInfo.yaw, &pitch_dummy);
				inputEnabled = false;
				ChangeState(ST_MOLE_GRAB_GO);
			}
			else if (this->nearToWall()) {
				//ChangePose(pose_wall_route);
				logic_manager->throwEvent(logic_manager->OnBreakWall, "");
				ChangeState(ST_MOLE_DESTROY);
			}
		}
	}
}

void player_controller_mole::UpdateMovingWithOther() {
	if (boxGrabbed.isValid()) {
		GET_COMP(box_t, boxGrabbed, TCompTransform);
		GET_COMP(box_p, boxGrabbed, TCompPhysics);

		if (____TIMER_IS_RUNNING(t_grab_hit)) {
			//VEC3 speed = cc->GetLastSpeed();
			//float amoung = speed.Dot(grabInfo.impact);
			//cc->AddSpeed(-grabInfo.impact * amoung * 10);
			//grabInfo.yaw = transform->getYaw();
		}
	}
}

void player_controller_mole::UpdateUnpossess() {
	CHandle h = CHandle(this);
	tags_manager.removeTag(h.getOwner(), getID("player"));

	if (boxGrabbed.isValid()) {
		LeaveBox();
	}
}

void player_controller_mole::DestroyWall() {
	energyDecreasal(10.0f);
	vector<CHandle> handles = SBB::readHandlesVector("wptsBreakableWall");
	handles.erase(handles.begin() + selectedWallToBreaki);
	getHandleManager<CEntity>()->destroyHandle(getEntityWallHandle(selectedWallToBreaki));
	SBB::postHandlesVector("wptsBreakableWall", handles);
	//ChangePose(pose_idle_route);
	ChangeState("idle");
}

void player_controller_mole::LeaveBox() {
	GET_COMP(box_t, boxGrabbed, TCompTransform);
	VEC3 posboxIni = box_t->getPosition();
	CEntity* p = myParent;
	TCompTransform* p_t = p->get<TCompTransform>();
	VEC3 posbox;
	posbox.x = posboxIni.x + p_t->getFront().x * 3;
	posbox.y = posboxIni.y - 2;
	posbox.z = posboxIni.z + p_t->getFront().z * 3;
	float angle = 0.0f;
	//TODO PHYSX OBJECT
	/*while (!box_t->setPosition(posbox)) {
		angle += 0.1;
		posbox.x = posboxIni.x + p_t->getFront().x * sin(angle) * 3;
		posbox.z = posboxIni.z + p_t->getFront().z * cos(angle) * 3;
	}*/
	GET_COMP(box_p, boxGrabbed, TCompPhysics);
	box_p->setPosition(posbox, box_t->getRotation());
	box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);
	//box_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, false); ojo! activar de nuevo
	box_p->setGravity(true);
	box_p->setKinematic(false);
	animController->ungrabObject();

	SBB::postBool(selectedBox, false);
	boxGrabbed = CHandle();
	mole_max_speed *= 2;
	//ChangePose(pose_idle_route);
	ChangeState(ST_MOLE_UNGRABBING);
	stopMovement();
	inputEnabled = false;
}

void player_controller_mole::LeavingBox()
{
	static float t_ungrab = SK_MOLE_TIME_TO_UNGRAB;
	t_ungrab -= getDeltaTime();
	if (t_ungrab < 0) {
		t_ungrab = SK_MOLE_TIME_TO_UNGRAB;
		ChangeState("idle");
		inputEnabled = true;
	}
}

bool player_controller_mole::nearToWall() {
	bool found = false;
	if (SBB::readHandlesVector("wptsBreakableWall").size() > 0) {
		float distMaxx = 6.0f;
		float distMaxz = 10.5f;
		for (int i = 0; !found && i < SBB::readHandlesVector("wptsBreakableWall").size(); i++) {
			CEntity * entTransform = this->getEntityWallHandle(i);
			TCompTransform * transformBox = entTransform->get<TCompTransform>();
			TCompName * nameBox = entTransform->get<TCompName>();
			VEC3 wpt = transformBox->getPosition();
			float disttowptx = fabsf(fabsf(wpt.x) - fabsf(getEntityTransform()->getPosition().x));
			float disttowptz = fabsf(fabsf(wpt.z) - fabsf(getEntityTransform()->getPosition().z));
			if (disttowptx < distMaxx && disttowptz < distMaxz) {
				distMaxx = disttowptx;
				distMaxz = disttowptz;
				selectedWallToBreaki = i;
				found = true;
			}
		}
	}
	return found;
}
bool player_controller_mole::nearToBox() {
	bool found = false;
	float distMax = 2.0f;
	float higher = -999.9f;
	string key_final = "";
	for (auto h : TCompBox::all_boxes) {
		if (!h.isValid()) continue;
		GET_COMP(tBox, h, TCompTransform);
		string key = ((CEntity*)h)->getName();
		VEC3 wpt = tBox->getPosition();
		float disttowpt = simpleDistXZ(wpt, getEntityTransform()->getPosition());
		if (disttowpt < distMax + 2 && wpt.y >= higher) {
			distMax = disttowpt;
			higher = wpt.y;
			boxNear = h;
			selectedBox = key;
			found = true;
		}
	}
	return found;
}
void player_controller_mole::GoToGrab()
{
	static float time_out_to_grab = MOLE_TIME_OUT_GO_GRAB;
	time_out_to_grab -= getDeltaTime();
	if (time_out_to_grab < 0) {
		inputEnabled = true;
		ChangeState("idle");
		time_out_to_grab = MOLE_TIME_OUT_GO_GRAB;
	}

	float yaw, pitch;
	transform->getAngles(&yaw, &pitch);
	float dist = simpleDistXZ(cc->GetPosition(), grabInfo.pos_to_grab);
	if (dist > epsilonPos) {
		// Go to target
		float deltaYaw = transform->getDeltaYawToAimTo(grabInfo.pos_to_grab);
		if (deltaYaw > epsilonYaw) transform->setAngles(yaw + 0.1f * deltaYaw, pitch);
		VEC3 dir = grabInfo.pos_to_grab - cc->GetPosition();
		dir.Normalize();
		cc->AddMovement(dir, player_max_speed * getDeltaTime());
		moving = true;
		ChangeCommonState("moving");
	}
	else {
		ChangeState(ST_MOLE_GRAB_FACE);
		time_out_to_grab = MOLE_TIME_OUT_GO_GRAB;
	}
}
void player_controller_mole::FaceToGrab()
{
	bool faced = turnTo(GETH_COMP(boxNear, TCompTransform));
	if (faced) {
		animController->grabObject(boxNear);
		animController->setState(AST_GRAB_DOWN);
		ChangeState(ST_MOLE_GRABBING_1);
	}
}

void player_controller_mole::GrabbingBox1()
{
	//Player bring hands to the box (static yet)
	//IK stuff

	//Provisional
	static float t_grab1 = SK_MOLE_TIME_TO_GRAB;
	t_grab1 -= getDeltaTime();
	if (t_grab1 < 0) {
		t_grab1 = SK_MOLE_TIME_TO_GRAB;
		ChangeState(ST_MOLE_GRABBING_2);
		animController->setState(AST_GRAB_UP);
	}
}

void player_controller_mole::GrabbingBox2()
{
	//Player return to idlebox pose
	//End IK Stuff progressively
	//Box track player hands

	//Provisional
	static float t_grab2 = SK_MOLE_TIME_TO_GRAB;
	t_grab2 -= getDeltaTime();
	if (t_grab2 < 0) {
		t_grab2 = SK_MOLE_TIME_TO_GRAB;
		ChangeState(ST_MOLE_GRAB);
		animController->setState(AST_GRAB_IDLE);
		GET_COMP(box_p, boxNear, TCompPhysics);
		box_p->setKinematic(true);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, true);
		box_p->setBehaviour(PHYS_BEHAVIOUR::eIGNORE_PLAYER, true);
		box_p->setGravity(false);

		inputEnabled = true;
	}
}

void player_controller_mole::GrabbedBox() {
	if (!SBB::readBool(selectedBox)) {
		SBB::postBool(selectedBox, true);
	}

	//TODO: Revisar esto, fisica caja
	GET_COMP(box_t, boxNear, TCompTransform);
	VEC3 posPlayer = transform->getPosition();
	VEC3 posBox = box_t->getPosition();
	grabInfo.dist = realDistXZ(posPlayer, posBox);
	grabInfo.y = posBox.y - posPlayer.y;
	grabInfo.yaw = transform->getDeltaYawToAimTo(posBox);
	grabInfo.last_correct_pos = posBox;

	energyDecreasal(5.0f);
	TMsgDamage dmg;
	dmg.modif = 0.5f;
	myEntity->sendMsg(dmg);
	boxGrabbed = boxNear;
	mole_max_speed /= 2;

	ChangeState("idle");
	logic_manager->throwEvent(logic_manager->OnPickupBox, "");
}

void player_controller_mole::onGrabHit(const TMsgGrabHit& msg)
{
	GET_COMP(box_p, boxGrabbed, TCompPhysics);
	GET_COMP(box_t, boxGrabbed, TCompTransform);
	VEC3 impact = VEC3(0.f, 0.f, 0.f);
	//dbg("--- IMPACT ------------------------------------------\n");
	for (int i = 0; i < msg.npoints; i++) {
		dbg("Point %d, pos = (%f,%f,%f)\n---\n", i, VEC3_VALUES(msg.points[i]));
		dbg("Normal = (%f,%f,%f)\n", VEC3_VALUES(msg.normals[i]));
		dbg("Separation = %f\n", msg.separations[i]);
		dbg("---\n");
		float amoung = msg.separations[i] > 0 ? msg.separations[i] : msg.separations[i] * -2.f;
		VEC3 local_corr = msg.normals[i] * amoung;
		impact = movementUnion(impact, local_corr);
		Debug->DrawLine(msg.points[i], msg.points[i] + local_corr + VEC3(0, 2.f, 0), VEC3(1, 1, 0), 10.f);
	}
	//dbg("------------------------------------- FIN IMPACT ----\n");
	VEC3 dif = grabInfo.last_correct_pos - box_t->getPosition();
	impact.y = 0;
	impact.Normalize();
	grabInfo.impact = impact;

	VEC3 boxPos = box_t->getPosition();
	impact.Normalize();
	Debug->DrawLine(boxPos, impact + VEC3(0.f, 0.2f, 0.f), 1.f, VEC3(0, 0, 1), 10.f);

	____TIMER_RESET_(t_grab_hit);
	ChangeState(ST_MOLE_GRABBING_IMPACT);
}

void player_controller_mole::GrabbingImpact()
{
	//GET_COMP(box_p, boxGrabbed, TCompPhysics);
	//cc->AddMovement(grabInfo.impact);
	//transform->addPosition(grabInfo.impact);
	//box_p->AddMovement(grabInfo.impact);
	//float yaw = transform->getYaw();
	//float deltaYaw = grabInfo.yaw - yaw;
	//transform->setYaw(yaw + deltaYaw*1.2f);
	//grabInfo.last_correct_pos = transform->getPosition();
	//grabInfo.impact = VEC3(0.f, 0.f, 0.f);
	ChangeState(ST_MOLE_GRABBING_IMPACT_1);
	cc->AddImpulse(grabInfo.impact* 4.f);
	stopMovement();
	inputEnabled = false;
}

void player_controller_mole::GrabbingImpact1()
{
	//stopMovement();
	bool recovered_control = ____TIMER__END_(t_grab_hit);
	if (recovered_control) {
		ChangeState(ST_MOLE_GRABBING_IMPACT_2);
	}
	else {
		____TIMER__UPDATE_(t_grab_hit);
	}
}
void player_controller_mole::GrabbingImpact2()
{
	//UpdateMovingWithOther();
	inputEnabled = true;
	ChangeState("idle");
}

void player_controller_mole::InitControlState() {
	CHandle h = CHandle(this);
	tags_manager.addTag(h.getOwner(), getID("player"));
	ChangeState("idle");
}

void player_controller_mole::update_msgs()
{
	ui.addTextInstructions("Left Shift            -> Exit possession State");
	ui.addTextInstructions("Click Left Mouse      -> Grab/Throw near Box or Break Wall");
}

void player_controller_mole::ChangeCommonState(std::string st)
{
	if (st == "moving") {
		SET_ANIM_MOLE(AST_MOVE);
	}
	else if (st == "running") {
		SET_ANIM_MOLE(AST_RUN);
	}
	else if (st == "jumping") {
		SET_ANIM_MOLE(AST_JUMP);
	}
	else if (st == "idle") {
		SET_ANIM_MOLE(AST_IDLE);
	}
}
bool player_controller_mole::canJump() {
	bool ascending = cc->GetLastSpeed().y > 0.1f;
	bool descending = cc->GetLastSpeed().y < -0.1f;
	return !boxGrabbed.isValid() && !ascending && !descending;
}