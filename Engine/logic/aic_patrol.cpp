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
	AddState("chase", (statehandler)&aic_patrol::ChaseState);
	AddState("idlewar", (statehandler)&aic_patrol::IdleWarState);
	AddState("combat", (statehandler)&aic_patrol::CombatState);
	AddState("oleft", (statehandler)&aic_patrol::OLeftState);
	AddState("oright", (statehandler)&aic_patrol::ORightState);
	AddState("selectrole", (statehandler)&aic_patrol::SelectRoleState);

	//add point to wpts vector
	wpts.push_back(VEC3(-9, 0, -9));
	wpts.push_back(VEC3(-9, 0, 9));
	wpts.push_back(VEC3(9, 0, 9));
	wpts.push_back(VEC3(9, 0, -9));

	curwpt = 0;
	distance_combat = 3.0f;
	idle_war_wait = 1000;
	combat_wait = 2000;

	entity->transform.setPosition(VEC3((float)(rand() % 10), 0, (float)(rand() % 10)));

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

	if (entity->transform.isHalfConeVision(player->transform.getPosition(), deg2rad(70))) {
		ChangeState("chase");
	}
	else if (abs(distance) > 0.5f) {
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

// Selecting role
void aic_patrol::SelectRoleState() {

	int free_slots = shared_board->read("attacking_slots");

	if (free_slots > 0) {
		free_slots--;
		shared_board->write("attacking_slots", free_slots);
		ChangeState("chase");
	}
	else {
		dbg("No hay slots vacíos, sigo patrullando!!");
		ChangeState("seekwpt");
	}
}

// Chasing player
void aic_patrol::ChaseState()
{
	VEC3 front = entity->transform.getFront();
	VEC3 position = entity->transform.getPosition();
	VEC3 target = player->transform.getPosition();

	float distance = (target.x - position.x) + (target.z - position.z);
	float delta_yaw = entity->transform.getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		entity->transform.getAngles(&yaw, &pitch);

		entity->transform.setAngles(yaw + delta_yaw*0.005f, pitch);
	}

	entity->transform.setPosition(VEC3(position.x + front.x*0.005f, position.y, position.z + front.z*0.005f));	

	if (abs(distance) < distance_combat) {
		ChangeState("idlewar");
	}
	
}

// Starting the fight
void aic_patrol::IdleWarState()
{
	VEC3 front = entity->transform.getFront();
	VEC3 position = entity->transform.getPosition();
	VEC3 target = player->transform.getPosition();

	float distance = (target.x - position.x) + (target.z - position.z);

	// El player se aleja 2 veces la distancia de combate, vuelvo a patrullar
	if (abs(distance) > (distance_combat * 2.0)) {
		ChangeState("chase");
	}
	// Me oriento hacia el player
	else {

		float delta_yaw = entity->transform.getDeltaYawToAimTo(target);

		if (abs(delta_yaw) > 0.001f) {
			float yaw, pitch;
			entity->transform.getAngles(&yaw, &pitch);

			entity->transform.setAngles(yaw + delta_yaw*0.005f, pitch);
		}
	}

	idle_war_wait--;
	// Si la espera termina, decido de manera random el siguiente estado
	if (idle_war_wait < 0) {

		idle_war_wait = 1000;
		int next_state = rand() % 4;

		switch (next_state) {
		case 0:
			ChangeState("combat");
			break;
		case 1:
			ChangeState("oleft");
			break;
		case 2:
			ChangeState("oright");
			break;
		case 3:
			ChangeState("idlewar");
			break;
		default:
			fatal("UNKNOWN STATE");
		}
	}	
}

// Fighting the player
void aic_patrol::CombatState()
{
	VEC3 front = entity->transform.getFront();
	VEC3 position = entity->transform.getPosition();
	VEC3 target = player->transform.getPosition();

	// Me oriento hacia el player
	float delta_yaw = entity->transform.getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		entity->transform.getAngles(&yaw, &pitch);
		entity->transform.setAngles(yaw + delta_yaw*0.005f, pitch);
	}

	combat_wait--;
	// Si la espera termina, ataco al player
	if (combat_wait < 0) {
		combat_wait = 2000;
		dbg("ESTOY ATACANDO AL PLAYER!!\n");
		// Done
		ChangeState("idlewar");
	}
}

// Orbit left
void aic_patrol::OLeftState()
{
	// Orbitamos 10 grados a la izquierda
	Orbit(-20.0f);

	// Done
	ChangeState("idlewar");
}

// Orbit right
void aic_patrol::ORightState()
{
	// Orbitamos 10 grados a la derecha
	Orbit(20.0f);

	// Done
	ChangeState("idlewar");
}

// Orbit function
void aic_patrol::Orbit(float angle) {

	VEC3 position = entity->transform.getPosition();
	VEC3 target = player->transform.getPosition();
	float angle_rad = deg2rad(angle);

	// Calculo del seno y coseno del angulo
	float s = sin(angle_rad);
	float c = cos(angle_rad);

	// Vector al centro de la circunferencia
	position.x = position.x - target.x;
	position.z = position.z - target.z;

	// Rotacion del punto
	float new_x = position.x * c - position.z * s;
	float new_y = position.x * s + position.z * c;

	// Revertir translacion
	position.x = new_x + target.x;
	position.z = new_y + target.z;

	// Cambio de posicion y reorientacion
	entity->transform.setPosition(position);

	float delta_yaw = entity->transform.getDeltaYawToAimTo(target);
	float yaw, pitch;
	entity->transform.getAngles(&yaw, &pitch);
	entity->transform.setAngles(yaw + delta_yaw, pitch);
}