#include "mcv_platform.h"
#include "player_controller.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "components\entity_tags.h"

#include "components\comp_msgs.h"


void player_controller::Init() {
	om = getHandleManager<player_controller>();	//player

	DeleteState("jumping");
	DeleteState("falling");

	AddState("doublefalling", (statehandler)&player_controller::DoubleFalling);		//needed to disable double jump on falling
	AddState("doublejump", (statehandler)&player_controller::DoubleJump);

	AddState("falling", (statehandler)&player_controller::Falling);
	AddState("jumping", (statehandler)&player_controller::Jumping);


	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	ChangeState("idle");
}

void player_controller::DoubleJump()
{
	UpdateDirection();
	UpdateMovDirection();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	jspeed -= gravity*getDeltaTime();
	if (jspeed <= 0.1f) {
		jspeed = 0.0f;
		ChangeState("doublefalling");
	}
}

void player_controller::DoubleFalling() {
	UpdateDirection();
	UpdateMovDirection();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	jspeed -= gravity*getDeltaTime();

	if (player_position.y <= 0) {
		onGround = true;
		jspeed = 0.0f;
		directionJump = VEC3(0, 0, 0);
		ChangeState("idle");
	}
}

void player_controller::Jumping()
{
	UpdateDirection();
	UpdateMovDirection();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	//dbg("deltatime: %f\n", getDeltaTime());
	jspeed -= gravity*getDeltaTime();
	//dbg("jspeed: %f\n", jspeed);

	if (jspeed <= 0.1f) {
		jspeed = 0.0f;
		ChangeState("falling");
	}
	if (Input.IsKeyPressedDown(DIK_SPACE)) {
		jspeed = jimpulse;
		ChangeState("doublejump");
	}
}

void player_controller::Falling()
{
	UpdateDirection();
	UpdateMovDirection();
	SetMyEntity();
	TCompTransform* player_transform = myEntity->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	jspeed -= gravity*getDeltaTime();

	if (Input.IsKeyPressedDown(DIK_SPACE)) {
		jspeed = jimpulse;
		ChangeState("doublejump");
	}

	if (player_position.y <= 0) {
		onGround = true;
		jspeed = 0.0f;
		directionJump = VEC3(0, 0, 0);
		ChangeState("idle");
	}
}
