#include <windows.h>
#include "mcv_platform.h"
#include "aic_patrol.h"

void aic_patrol::Init(TEntity* new_entity)
{

	entity = new_entity;

	// insert all states in the map
	AddState("idle",(statehandler)&aic_patrol::IdleState);
	AddState("seekwpt",(statehandler)&aic_patrol::SeekWptState);
	AddState("nextwpt",(statehandler)&aic_patrol::NextWptState);

	//add point to wpts vector
	wpts.push_back(VEC3(-5, 0, -5));
	wpts.push_back(VEC3(-5, 0, 5));
	wpts.push_back(VEC3(5, 0, 5));
	wpts.push_back(VEC3(5, 0, -5));

	curwpt = 0;

	entity->transform.setPosition(VEC3(10.0f, 0, 0));

	// reset the state
	ChangeState("idle");
}


void aic_patrol::IdleState()
{
	VEC3 target = wpts[curwpt];
	float delta_yaw = entity->transform.getDeltaYawToAimTo(target);
	float yaw, pitch;
	entity->transform.getAngles(&yaw, &pitch);
	entity->transform.setAngles(yaw + delta_yaw, pitch);
	ChangeState("seekwpt");
}


void aic_patrol::SeekWptState()
{
	float distance = (wpts[curwpt].x - entity->transform.getPosition().x) + (wpts[curwpt].z - entity->transform.getPosition().z);

	if (abs(distance) > 0.5f) {
		VEC3 front = entity->transform.getFront();
		VEC3 position = entity->transform.getPosition();

		entity->transform.setPosition(VEC3(position.x + front.x*0.005f, position.y, position.z + front.z*0.005f));

	}
	else {
		entity->transform.setPosition(wpts[curwpt]);
		curwpt = (curwpt + 1) % 4;
		ChangeState("nextwpt");
	}
}

// Seeking the next wpt
void aic_patrol::NextWptState()
{
	VEC3 front = entity->transform.getFront();
	VEC3 target = wpts[curwpt];

	float delta_yaw = entity->transform.getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		entity->transform.getAngles(&yaw, &pitch);

		entity->transform.setAngles(yaw + delta_yaw*0.005f, pitch);
	}
	else {
		ChangeState("seekwpt");
	}
}