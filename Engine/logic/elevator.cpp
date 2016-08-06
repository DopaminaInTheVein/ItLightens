#include "mcv_platform.h"
#include "elevator.h"
#include "components/comp_msgs.h"

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

	return true;
}

void elevator::onCreate(const TMsgEntityCreated&)
{
	CHandle parent = CHandle(this).getOwner();
	CEntity* eMe = parent;
	assert(eMe);
	TCompTransform * transform = eMe->get<TCompTransform>();
	assert(transform);

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
	physics = eMe->get<TCompPhysics>();
	physics->setKinematic(true);
}

void elevator::update(float elapsed)
{
	if (!isInRoom(CHandle(this).getOwner()))return;
	updateMove();
	notifyNewState();
	Debug->DrawLine(transform->getPosition(), targetDown);
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
		string nameElevator = string(((CEntity*)myEntity)->getName());
		switch (state) {
		case UP:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorUp, nameElevator, myEntity);
			break;
		case DOWN:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorDown, nameElevator, myEntity);
			break;
		case GOING_UP:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorGoingUp, nameElevator, myEntity);
			break;
		case GOING_DOWN:
			logic_manager->throwEvent(CLogicManagerModule::EVENT::OnElevatorGoingDown, nameElevator, myEntity);
			break;
		}
	}
	prevState = state;
}

bool elevator::getUpdateInfo() {
	//My Info
	myEntity = compBaseEntity;

	CEntity* eMe = myEntity;
	transform = eMe->get<TCompTransform>();
	if (!transform) return false;
	physics = eMe->get<TCompPhysics>();
	if (!physics) return false;

	return true;
}

void elevator::onElevatorAction(const TMsgActivate& msg)
{
	state = (state == UP || state == GOING_UP) ? GOING_DOWN : GOING_UP;
	notifyNewState();
}