#include "mcv_platform.h"
#include "elevator.h"
#include "components/comp_msgs.h"

#define DIST_DISABLE_PITCH_SQ	9.f

bool elevator::load(MKeyValue& atts)
{
	speedUp = atts.getFloat("speedUp", 7.f);
	speedDown = atts.getFloat("speedDown", speedUp);

	//May be indicated target only (and current position) or target_up and down
	VEC3 target = atts.getPoint("target");
	//Up and Down
	if (isZero(target)) {
		targetUp = atts.getPoint("target_up");
		targetDown = atts.getPoint("target_down");
	}
	//Target and position
	else {
		targetDown = targetUp = atts.getPoint("target"); // Both as targets, because onCreate we will set targetUp/down as initial position
	}
	state_init = atts.getInt("state", -1);
	epsilonTarget = 0.01f;
	return true;
}
/*	VEC3 targetUp, targetDown;
	float speedUp;
	float speedDown;
	float epsilonTarget;
	float lastSpeed = 0.0f;
	*/
bool elevator::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("speedUp", speedUp);
	atts.put("speedDown", speedDown);
	atts.put("target_up", targetUp);
	atts.put("target_down", targetDown);
	atts.put("state", state);
	return true;
}

void elevator::onCreate(const TMsgEntityCreated&)
{
	CHandle parent = CHandle(this).getOwner();
	CEntity* eMe = parent;
	assert(eMe);
	TCompTransform * transform = eMe->get<TCompTransform>();
	assert(transform);

	if (state_init < 0) {
		if (targetUp == targetDown) {
			VEC3 initialPos = transform->getPosition();
			if (initialPos.y > targetUp.y) {
				targetUp = initialPos;
				prevState = state = UP;
			}
			else {
				targetDown = initialPos;
				prevState = state = DOWN;
			}
		}
	}
	else {
		prevState = state = (eElevatorState)state_init;
	}
	physics = eMe->get<TCompPhysics>();
	physics->setKinematic(true);
}

void elevator::update(float elapsed)
{
	if (!isInRoom(CHandle(this).getOwner()))return;
	updateMove();
	notifyNewState();
	Debug->DrawLine(transform->getPosition(), targetDown);
	updatePlayerNear();
}

//Move door and set OPENED or CLOSED if movement ends
void elevator::updateMove()
{
	if (state == UP || state == DOWN) return;

	VEC3 target;
	float drag = 0.99f;
	float drag_i = 0.01f;
	float speed;
	eElevatorState targetState;
	if (state == GOING_UP) {
		target = targetUp;
		speed = speedUp;
		targetState = UP;
	}
	else if (state == GOING_DOWN) {
		target = targetDown;
		speed = speedDown;
		targetState = DOWN;
	}
	lastSpeed = speed*drag_i + lastSpeed * drag;
	VEC3 delta = target - transform->getPosition();
	float moveAmount = min(lastSpeed * getDeltaTime(), delta.Length());
	PxRigidDynamic *rd = physics->getActor()->isRigidDynamic();

	if (rd) {
		//moveAmount = min(delta.Length(), moveAmount);
		delta.Normalize();
		PxTransform tmx = rd->getGlobalPose();
		VEC3 pos = PhysxConversion::PxVec3ToVec3(tmx.p);
		VEC3 nextPos = pos + delta * moveAmount;
		PxVec3 pxTarget = PhysxConversion::Vec3ToPxVec3(nextPos);
		rd->setKinematicTarget(PxTransform(pxTarget, tmx.q));
	}

	// Target has reached
	if (simpleDist(target, transform->getPosition()) < epsilonTarget) {
		state = targetState;
		lastSpeed = 0.0f;
		CHandle player = tags_manager.getFirstHavingTag(getID("player"));
		if (player.isValid()) {
			CEntity * ePlayer = player;
			TMsgSetControllable msg;
			msg.control = true;
			ePlayer->sendMsg(msg);
		}
	}
}

void elevator::notifyNewState()
{
	if (prevState != state) {
		string nameElevator = MY_NAME;
		CHandle me = MY_OWNER;
		switch (state) {
		case UP:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorUp, nameElevator, me);
			break;
		case DOWN:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorDown, nameElevator, me);
			break;
		case GOING_UP:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorGoingUp, nameElevator, me);
			break;
		case GOING_DOWN:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorGoingDown, nameElevator, me);
			break;
		}
	}
	prevState = state;
}

bool elevator::getUpdateInfo() {
	transform = GETH_MY(TCompTransform);
	if (!transform) return false;

	physics = GETH_MY(TCompPhysics);
	if (!physics) return false;

	if (!player.isValid() || !player.hasTag("player")) player = CPlayerBase::handle_player;
	if (!player.isValid()) return false;

	if (!camera.isValid()) camera = tags_manager.getFirstHavingTag("camera_main");
	if (!camera.isValid()) return false;

	player_tmx = GETH_COMP(player, TCompTransform);
	if (!player_tmx) return false;

	player_3rd = GETH_COMP(camera, TCompController3rdPerson);
	if (!player_3rd) return false;

	return true;
}

void elevator::onElevatorAction(const TMsgActivate& msg)
{
	state = (state == UP || state == GOING_UP) ? GOING_DOWN : GOING_UP;
	notifyNewState();
}

void elevator::updatePlayerNear()
{
	bool now_is_near = inSquaredRangeXZ_Y(transform->getPosition(), player_tmx->getPosition(), DIST_DISABLE_PITCH_SQ, 3.f);
	if (now_is_near) {
		player_3rd->SetPitchEnabled(false);
	}
	else {
		if (player_near) player_3rd->SetPitchEnabled(true);
	}
	player_near = now_is_near;
}