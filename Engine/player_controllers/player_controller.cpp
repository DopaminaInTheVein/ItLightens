#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
#include "render\static_mesh.h"
#include "app_modules\io\io.h"
#include "components/comp_msgs.h"
#include "ui\ui_interface.h"

#include "components\comp_charactercontroller.h"

#define DELTA_YAW_SELECTION		deg2rad(10)

void player_controller::Init() {
	om = getHandleManager<player_controller>();	//player

	DeleteState("jumping");
	DeleteState("falling");

	AddState("doublefalling", (statehandler)&player_controller::DoubleFalling);		//needed to disable double jump on falling
	AddState("doublejump", (statehandler)&player_controller::DoubleJump);

	AddState("falling", (statehandler)&player_controller::Falling);
	AddState("jumping", (statehandler)&player_controller::Jumping);
	AddState("toplus", (statehandler)&player_controller::AttractToPlus);
	AddState("tominus", (statehandler)&player_controller::AttractToMinus);

	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
	myEntity = myParent;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();

	pose_run	= getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_jump	= getHandleManager<TCompRenderStaticMesh>()->createHandle();
	pose_idle	= getHandleManager<TCompRenderStaticMesh>()->createHandle();

	pose_no_ev		= myEntity->get<TCompRenderStaticMesh>();		//defined on xml
	actual_render	= pose_no_ev;

	pose_no_ev.setOwner(myEntity);
	pose_idle.setOwner(myEntity);
	pose_run.setOwner(myEntity);
	pose_jump.setOwner(myEntity);

	TCompRenderStaticMesh *mesh;

	mesh = pose_idle;
	mesh->static_mesh = Resources.get("static_meshes/player_idle.static_mesh")->as<CStaticMesh>();

	mesh = pose_jump;
	mesh->static_mesh = Resources.get("static_meshes/player_jump.static_mesh")->as<CStaticMesh>();

	mesh = pose_run;
	mesh->static_mesh = Resources.get("static_meshes/player_run.static_mesh")->as<CStaticMesh>();

	actual_render->registerToRender();

	ChangeState("idle");
	controlEnabled = true;
	____TIMER__SET_ZERO_(timerDamaged);
}

bool player_controller::isDamaged() {
	PROFILE_FUNCTION("is damaged");
	return !____TIMER__END_(timerDamaged);
}

void player_controller::rechargeEnergy()
{
	PROFILE_FUNCTION("recharge_eergy");
	TCompLife *life = myEntity->get<TCompLife>();
	life->setMaxLife(max_life);
	TCompCharacterController *p = myEntity->get<TCompCharacterController>();
	PxController *cc = p->GetController();
	cc->resize(full_height);
	ChangePose(pose_idle);
}


void player_controller::ChangePose(CHandle new_pos_h)
{
	PROFILE_FUNCTION("change pose player");
	SetMyEntity();
	TCompLife *life = myEntity->get<TCompLife>();
	if (life->currentlife < evolution_limit) {
		SetCharacterController();
		new_pos_h = pose_no_ev;
		cc->GetController()->resize(min_height);	//update collider size to new form
	}

	TCompRenderStaticMesh *new_pose = new_pos_h;
	if (new_pose == actual_render) return;		//no change

	actual_render->unregisterFromRender();
	actual_render = new_pose;
	actual_render->registerToRender();
}

void player_controller::myUpdate() {
	PROFILE_FUNCTION("MY_update");
	SetMyEntity();
	TCompTransform *m = myEntity->get<TCompTransform>();
	
	/*
	//TESTING RAYCAST
	int hits = 0;
	SetCharacterController();
	VEC3 origin = PhysxConversion::PxExVec3ToVec3(cc->GetController()->getFootPosition());
	Debug->DrawLine(origin + m->getFront()*0.5f + VEC3(0,1,0), m->getFront(), 2.0f);
	PxQueryFilterData filterData = PxQueryFilterData();
	filterData.data.word0 = CPhysxManager::eGUARD;
	PxRaycastBuffer hit;

	if (PhysxManager->raycast(origin + m->getFront()*0.5f + VEC3(0, 0.5f, 0), m->getFront(), 2.0f, hit, filterData)) {
		hits = hit.hasAnyHits();
		Debug->LogRaw("player hits = %d to guard\n", hits);
	}
	//END TESTING RAYCAST
	*/

	____TIMER__UPDATE_(timerDamaged);
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 pos = player_transform->getPosition();
	if (!isDamaged()) {
		UpdatePossession();
	}
}

void player_controller::DoubleJump()
{
	PROFILE_FUNCTION("double jump");
	UpdateDirection();
	UpdateMovDirection();

	SetCharacterController();

	if (cc->GetYAxisSpeed() < 0.0f) {
		ChangeState("doublefalling");
	}
}

void player_controller::DoubleFalling() {
	PROFILE_FUNCTION("double falling");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();
	if (cc->OnGround()) {
		ChangeState("idle");
	}
}

void player_controller::Jumping()
{
	PROFILE_FUNCTION("jumping");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();

	if (cc->GetYAxisSpeed() <= 0.0f)
		ChangeState("falling");

	if (cc->OnGround()) {
		ChangeState("idle");
	}

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		cc->AddImpulse(VEC3(0.0f,jimpulse,0.0f));
		energyDecreasal(5.0f);
		ChangeState("doublejump");
	}
}

void player_controller::Falling()
{
	PROFILE_FUNCTION("falling");
	UpdateDirection();
	UpdateMovDirection();
	SetCharacterController();

	//Debug->LogRaw("%s\n", io->keys[VK_SPACE].becomesPressed() ? "true" : "false");

	if (io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed()) {
		cc->AddImpulse(VEC3(0.0f,jimpulse,0.0f));
		energyDecreasal(5.0f);
		ChangeState("doublejump");
	}

	if (cc->OnGround()) {
		ChangeState("idle");
	}
}

void player_controller::AttractToMinus() {
	PROFILE_FUNCTION("attract to minus");
	CEntity * entPoint = this->getMinusPointHandle(topolarizedminus);
	tominus = true;
	toplus = false;
	AttractMove(entPoint);
	ChangeState("idle");
}
void player_controller::AttractToPlus() {
	PROFILE_FUNCTION("attract to plus");
	CEntity * entPoint = this->getPlusPointHandle(topolarizedplus);
	tominus = false;
	toplus = true;
	AttractMove(entPoint);
	ChangeState("idle");
}

bool player_controller::nearMinus() {
	PROFILE_FUNCTION("near minus");
	if (topolarizedminus != -1) {
		return true;
	}
	else {
		bool found = false;
		if (SBB::readHandlesVector("wptsMinusPoint").size() > 0) {
			float distMax = 10.0f;
			for (int i = 0; !found && i < SBB::readHandlesVector("wptsMinusPoint").size(); i++) {
				CEntity * entTransform = this->getMinusPointHandle(i);
				TCompTransform * transformBox = entTransform->get<TCompTransform>();
				VEC3 wpt = transformBox->getPosition();
				float disttowpt = simpleDist(wpt, getEntityTransform()->getPosition());
				if (disttowpt < distMax) {
					distMax = disttowpt;
					topolarizedminus = i;
					found = true;
					polarizedMove = true;
				}
			}
		}
		return found;
	}
}
bool player_controller::nearPlus() {
	PROFILE_FUNCTION("near plus");
	if (topolarizedplus != -1) {
		return true;
	}
	else {
		bool found = false;
		if (SBB::readHandlesVector("wptsPlusPoint").size() > 0) {
			float distMax = 10.0f;
			for (int i = 0; !found && i < SBB::readHandlesVector("wptsPlusPoint").size(); i++) {
				CEntity * entTransform = this->getPlusPointHandle(i);
				TCompTransform * transformBox = entTransform->get<TCompTransform>();
				VEC3 wpt = transformBox->getPosition();
				float disttowpt = simpleDist(wpt, getEntityTransform()->getPosition());
				if (disttowpt < distMax) {
					distMax = disttowpt;
					topolarizedplus = i;
					found = true;
					polarizedMove = true;
				}
			}
		}
		return found;
	}
}

void player_controller::AttractMove(CEntity * entPoint) {
	PROFILE_FUNCTION("attract move");
	if (entPoint == nullptr) {
		return;
	}
	TCompTransform * entPointTransform = entPoint->get<TCompTransform>();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	VEC3 direction = entPointTransform->getPosition() - player_position;
	float drag = getDeltaTime();
	float drag_i = (1 - drag);

	if (polarizedMove) polarizedCurrentSpeed = drag_i*polarizedCurrentSpeed + drag*player_max_speed;
	else polarizedCurrentSpeed = drag_i*polarizedCurrentSpeed - drag*player_max_speed;

	float multiplier = polarizedCurrentSpeed * 1.5f;

	float tox = min(fabsf(direction.x)*multiplier, fabsf(player_position.x - entPointTransform->getPosition().x));
	float toy = min(fabsf(direction.y)*multiplier, fabsf(player_position.y - entPointTransform->getPosition().y));
	float toz = min(fabsf(direction.z)*multiplier, fabsf(player_position.z - entPointTransform->getPosition().z));

	if (direction.x < 0) {
		tox *= -1;
	}
	if (direction.z < 0) {
		toz *= -1;
	}
	if (direction.y < 0) {
		toy *= -1;
	}

	SetCharacterController();
	cc->AddImpulse(VEC3(tox, toy, toz));
}

void player_controller::UpdateMoves()
{
	PROFILE_FUNCTION("update moves");
	SetMyEntity();
	SetCharacterController();

	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();

	VEC3 direction = directionForward + directionLateral;

	CEntity * camera_e = camera;
	TCompTransform* camera_comp = camera_e->get<TCompTransform>();

	direction.Normalize();

	float yaw, pitch;
	camera_comp->getAngles(&yaw, &pitch);
	float new_x, new_z;

	new_x = direction.x * cosf(yaw) + direction.z*sinf(yaw);
	new_z = -direction.x * sinf(yaw) + direction.z*cosf(yaw);

	direction.x = new_x;
	direction.z = new_z;

	direction.Normalize();

	float new_yaw = player_transform->getDeltaYawToAimDirection(direction);

	player_transform->getAngles(&yaw, &pitch);

	player_transform->setAngles(new_yaw + yaw, pitch);		//control rotate transfom, not needed for the character controller

	//Set current velocity with friction
	float drag = 2.5f*getDeltaTime();
	float drag_i = (1 - drag);

	if (moving) player_curr_speed = drag_i*player_curr_speed + drag*player_max_speed;
	else player_curr_speed = drag_i*player_curr_speed - drag*player_max_speed;

	if (player_curr_speed < 0) {
		player_curr_speed = 0.0f;
		directionForward = directionLateral = VEC3(0, 0, 0);
	}

	if (cc->OnGround()) {
		ChangePose(pose_run);
	}
	else {
		ChangePose(pose_jump);
	}

	if (player_curr_speed == 0.0f) ChangePose(pose_idle);


	
	cc->AddMovement(direction , player_curr_speed);
}

void player_controller::UpdateInputActions()
{
	PROFILE_FUNCTION("update input actions");
	if ((io->keys['1'].isPressed() || io->joystick.button_L.isPressed()) && nearMinus()) {
		energyDecreasal(getDeltaTime()*0.05f);
		ChangeState("tominus");
	}
	else if ((io->keys['2'].isPressed() || io->joystick.button_R.isPressed()) && nearPlus()) {
		energyDecreasal(getDeltaTime()*0.05f);
		ChangeState("toplus");
	}
	else if (polarizedCurrentSpeed > .2f) {
		energyDecreasal(getDeltaTime()*0.1f);
		polarizedMove = false;
		CEntity * entPoint = nullptr;
		if (tominus) {
			entPoint = this->getMinusPointHandle(topolarizedminus);
		}
		else if (toplus) {
			entPoint = this->getPlusPointHandle(topolarizedplus);
		}
		AttractMove(entPoint);
	}
	else if ((io->mouse.left.becomesReleased() || io->joystick.button_X.becomesReleased()) && nearStunable()) {
		energyDecreasal(5.0f);
		// Se avisa el ai_poss que ha sido stuneado
		CEntity* ePoss = currentStunable;
		TMsgAISetStunned msg;
		msg.stunned = true;
		ePoss->sendMsg(msg);
	}
	else if (io->mouse.left.isPressed() || io->joystick.button_X.isPressed()) {
		SetMyEntity();
		TCompTransform* player_transform = myEntity->get<TCompTransform>();
		vector<CHandle> ptsRecover = SBB::readHandlesVector("wptsRecoverPoint");
		for (CEntity * ptr : ptsRecover) {
			TCompTransform * ptr_trn = ptr->get<TCompTransform>();
			if (3 > simpleDist(ptr_trn->getPosition(), player_transform->getPosition())) {
				energyDecreasal(-15.0f*getDeltaTime());
			}
		}
	}
	else {
		topolarizedplus = -1;
		topolarizedminus = -1;
		polarizedCurrentSpeed = 0.0f;
	}
}

void player_controller::SetCharacterController()
{
	PROFILE_FUNCTION("set cc");
	SetMyEntity();
	cc = myEntity->get<TCompCharacterController>();
}

float CPlayerBase::possessionCooldown;
//Possession
void player_controller::UpdatePossession() {
	PROFILE_FUNCTION("update poss");
	recalcPossassable();
	if (currentPossessable.isValid()) {
		if (io->keys[VK_SHIFT].becomesPressed() || io->joystick.button_Y.becomesPressed()) {
			// Se avisa el ai_poss que ha sido poseído
			CEntity* ePoss = currentPossessable;
			TMsgAISetPossessed msg;
			msg.possessed = true;
			ePoss->sendMsg(msg);
			possessionCooldown = 1.0f;
			// Camara Nueva
			CEntity * player_e = tags_manager.getFirstHavingTag(getID("player"));
			TMsgSetTarget msgTarg;
			msgTarg.target = ePoss;
			player_e->sendMsg(msgTarg);

			//Se desactiva el player
			controlEnabled = false;
			SBB::postBool("possMode", true);

			//TODO: Desactivar render
			SetCharacterController();
			cc->SetActive(false);
			cc->GetController()->setPosition(PxExtendedVec3(0, 200, 0));
			TCompTransform *t = myEntity->get<TCompTransform>();
			t->setPosition(VEC3(0, 200, 0));
			player_curr_speed = 0;
		}
	}
}

// Recalcula el mejor candidato para poseer
void player_controller::recalcPossassable() {
	PROFILE_FUNCTION("recalc possessable");
	float minDeltaYaw = FLT_MAX;
	float minDistance = FLT_MAX;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	currentPossessable = CHandle();
	VHandles possessables = tags_manager.getHandlesByTag(getID("AI_poss"));
	for (CHandle hPoss : possessables) {
		CEntity* ePoss = hPoss;
		TCompTransform* tPoss = ePoss->get<TCompTransform>();
		VEC3 posPoss = tPoss->getPosition();
		float dist = realDist(player_position, posPoss);
		if (dist < possessionReach) {
			float yaw = player_transform->getDeltaYawToAimTo(posPoss);
			yaw = abs(yaw);
			if (yaw > deg2rad(90)) continue;

			float improvementDeltaYaw = minDeltaYaw - yaw;
			bool isBetter = false;
			if (improvementDeltaYaw > DELTA_YAW_SELECTION) {
				isBetter = true;
			}
			else if (improvementDeltaYaw < DELTA_YAW_SELECTION) {
				isBetter = false;
			}
			else {
				isBetter = dist < minDistance;
			}
			if (isBetter) {
				currentPossessable = hPoss;
				minDeltaYaw = abs(yaw);
				minDistance = dist;
			}
		}
	}

	//Debug
	if (currentPossessable.isValid()) {
		CEntity* ePoss = currentPossessable;
		TCompTransform* tPoss = ePoss->get<TCompTransform>();
		VEC3 posPoss = tPoss->getPosition();
		Debug->DrawLine(posPoss + VEC3(-0.1f, 1.5f, -0.1f), posPoss + VEC3(0.1f, 1.5f, 0.1f), BLUE);
		Debug->DrawLine(posPoss + VEC3(0.1f, 1.5f, -0.1f), posPoss + VEC3(-0.1f, 1.5f, 0.1f), BLUE);
	}
}

// Calcula el mejor candidato para stunear
bool player_controller::nearStunable() {
	PROFILE_FUNCTION("near stunnable");
	float minDeltaYaw = FLT_MAX;
	float minDistance = FLT_MAX;
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	currentStunable = CHandle();
	VHandles stuneables = tags_manager.getHandlesByTag(getID("AI_poss"));
	for (CHandle hPoss : stuneables) {
		CEntity* ePoss = hPoss;
		TCompTransform* tPoss = ePoss->get<TCompTransform>();
		VEC3 posPoss = tPoss->getPosition();
		float dist = realDist(player_position, posPoss);
		if (dist < possessionReach) {
			float yaw = player_transform->getDeltaYawToAimTo(posPoss);
			yaw = abs(yaw);
			if (yaw > deg2rad(90)) continue;

			float improvementDeltaYaw = minDeltaYaw - yaw;
			bool isBetter = false;
			if (improvementDeltaYaw > DELTA_YAW_SELECTION) {
				isBetter = true;
			}
			else if (improvementDeltaYaw < DELTA_YAW_SELECTION) {
				isBetter = false;
			}
			else {
				isBetter = dist < minDistance;
			}
			if (isBetter) {
				currentStunable = hPoss;
				minDeltaYaw = abs(yaw);
				minDistance = dist;
			}
		}
	}

	//Debug
	if (currentStunable.isValid()) {
		return true;
	}
	return false;
}

void player_controller::onLeaveFromPossession(const TMsgPossessionLeave& msg) {
	PROFILE_FUNCTION("on leave poss");
	// Handles y entities necesarias
	CHandle  hMe = CHandle(this).getOwner();
	CEntity* eMe = hMe;
	CHandle hPlayer = tags_manager.getFirstHavingTag(getID("player"));
	CEntity* ePlayer = hPlayer;
	

	//Colocamos el player
	SetCharacterController();
	VEC3 pos = msg.npcPos + VEC3(0.0f, cc->GetHeight(), 0.0f);	//to be sure the collider will be above the ground, add height from collider, origin on center shape
	TCompTransform* tMe = eMe->get<TCompTransform>();
	cc->GetController()->setPosition(PhysxConversion::Vec3ToPxExVec3(pos + msg.npcFront * DIST_LEAVING_POSSESSION));	//set collider position
	tMe->setPosition(msg.npcPos + msg.npcFront * DIST_LEAVING_POSSESSION);												//set render position
	cc->SetActive(true);


	//Set 3rd Person Controller
	TMsgSetTarget msg3rdController;
	msg3rdController.target = hMe;
	ePlayer->sendMsg(msg3rdController);

	//Set Camera
	camera = CHandle(ePlayer);

	//Habilitamos control
	controlEnabled = true;

	//Notificamos presencia de Player
	SBB::postBool("possMode", false);
}

void player_controller::update_msgs()
{
	PROFILE_FUNCTION("updat mesgs");
	ui.addTextInstructions("Press 'l-shift' to possess someone\n");
}

void player_controller::onDamage(const TMsgDamage& msg) {
	PROFILE_FUNCTION("onDamage");
	switch (msg.dmgType) {
	case LASER:
		____TIMER_RESET_(timerDamaged);
		break;
	case WATER:
		____TIMER_RESET_(timerDamaged);
		break;
	}
}

void player_controller::onWirePass(const TMsgWirePass & msg)
{
	PROFILE_FUNCTION("onWirepass");
	ui.addTextInstructions("\n Press 'E' to pass by the wire\n");

	if (io->keys['E'].becomesPressed()) {
		SetMyEntity();
		SetCharacterController();
		cc->GetController()->setPosition(PhysxConversion::Vec3ToPxExVec3(msg.dst));
	}
}

void player_controller::onCanRec(const TMsgCanRec & msg)
{
	PROFILE_FUNCTION("onCanrec");
	ui.addTextInstructions("\n Press 'E' to recharge energy\n");

	if (io->keys['E'].becomesPressed()) {
		rechargeEnergy();
	}
}
