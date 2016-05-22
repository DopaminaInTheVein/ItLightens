#include "mcv_platform.h"
#include "elevator.h"
#include "components/comp_msgs.h"

bool elevator::load(MKeyValue& atts)
{
	speedUp = atts.getFloat("speedOpening", 5.f);
	speedDown = atts.getFloat("speedClosing", speedUp);
	targetDown = targetUp = atts.getPoint("target"); // Both as targets, because onCreate we will set targetUp/down as initial position
	epsilonTarget = 0.001f;
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
	if (initialPos.y > targetUp.y) targetUp = initialPos;
	else targetDown = initialPos;
	
	physics = eMe->get<TCompPhysics>();
	physics->setKinematic(true);
}

void elevator::update(float elapsed)
{
	if (getUpdateInfo()) {
		updateCinematicState();
		updateMove();
		notifyNewState();
	}
}

//Set opening or closing 
void elevator::updateCinematicState()
{
	//switch (magneticBehaviour) {
	//case MB_NONE:
	//	cinematicState = locked ? CS_CLOSING : CS_OPENING;
	//	break;
	//case MB_OPENING:
	//	cinematicState = CS_OPENING;
	//	break;
	//case MB_CLOSING:
	//	cinematicState = CS_CLOSING;
	//	break;
	//}
}

//Move door and set OPENED or CLOSED if movement ends
void elevator::updateMove()
{
	//VEC3 target;
	//float speed;
	//eCinematicState targetState;

	//// Set target
	//if (cinematicState == CS_OPENING) {
	//	target = targetOpened;
	//	speed = speedOpening;
	//	targetState = CS_OPENED;
	//}
	//else {
	//	target = targetClosed;
	//	speed = speedClosing;
	//	targetState = CS_CLOSED;
	//}

	//// Door has reached targed
	//if (simpleDist(target, transform->getPosition()) < epsilonTarget) {
	//	cinematicState = targetState;
	//}
	////Door has to move
	//else {
	//	VEC3 delta = target - transform->getPosition();
	//	float moveAmount = speed * getDeltaTime();
	//	PxRigidDynamic *rd = physics->getActor()->isRigidDynamic();
	//	if (rd) {
	//		moveAmount = min(delta.Length(), moveAmount);
	//		VEC3 nextPos = transform->getPosition() + delta * moveAmount;
	//		PxTransform tmx = rd->getGlobalPose();
	//		PxVec3 pxTarget = PhysxConversion::Vec3ToPxVec3(nextPos);
	//		rd->setKinematicTarget(PxTransform(pxTarget, tmx.q));
	//	}
	//}
}

void elevator::notifyNewState()
{
	//if (prevCinematicState != cinematicState) {
	//	switch (cinematicState) {
	//	case CS_OPENING:
	//		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorOpening, "", myEntity);
	//		break;
	//	case CS_OPENED:
	//		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorOpened, "", myEntity);
	//		break;
	//	case CS_CLOSED:
	//		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorClosed, "", myEntity);
	//		break;
	//	case CS_CLOSING:
	//		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnDoorClosing, "", myEntity);
	//		break;
	//	}
	//}
	//prevCinematicState = cinematicState;
}

bool elevator::getUpdateInfo() {
	//My Info
	myEntity = CHandle(this).getOwner();
	if (!myEntity.isValid()) return false;
	CEntity* eMe = myEntity;
	transform = eMe->get<TCompTransform>();
	if (!transform) return false;
	physics = eMe->get<TCompPhysics>();
	if (!physics) return false;

	return true;
}

//void elevator::onElevatorAction(const TMsgElevator& msg)
//{
//	locked = msg.locked;
//}