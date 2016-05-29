#include "mcv_platform.h"
#include "elevator.h"
#include "components/comp_msgs.h"

bool elevator::load(MKeyValue& atts)
{
	speedUp = atts.getFloat("speedOpening", 1.f);
	speedDown = atts.getFloat("speedClosing", speedUp);
	targetDown = targetUp = atts.getPoint("target"); // Both as targets, because onCreate we will set targetUp/down as initial position
	epsilonTarget = 0.1f;
	return true;
}

void elevator::onCreate(const TMsgEntityCreated&)
{
	CHandle parent = CHandle(this).getOwner();
	CEntity* eMe = parent;
	assert(eMe);
	TCompTransform * transform = eMe->get<TCompTransform>();
	assert(transform);

	VEC3 initialPos = transform->getPosition();
	if (initialPos.y > targetUp.y) {
		targetUp = initialPos;
		prevState = state = UP;
	}
	else {
		targetDown = initialPos;
		prevState = state = DOWN;
	}

	physics = eMe->get<TCompPhysics>();
	physics->setKinematic(true);
}

void elevator::update(float elapsed)
{
	updateMove();
	notifyNewState();
}

//Move door and set OPENED or CLOSED if movement ends
void elevator::updateMove()
{
	if (state == UP || state == DOWN) return;

	VEC3 target;
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

	VEC3 delta = target - transform->getPosition();
	float moveAmount = speed * getDeltaTime();
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