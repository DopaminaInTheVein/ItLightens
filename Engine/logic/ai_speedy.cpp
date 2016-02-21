#include "mcv_platform.h"<
#include "ai_speedy.h"

void ai_speedy::Init(CEntity *new_entity, CEntity* new_player)
{
	// insert all states in the map
	AddState("idle", (statehandler)&ai_speedy::IdleState);
	AddState("nextwpt", (statehandler)&ai_speedy::NextWptState);
	AddState("seekwpt", (statehandler)&ai_speedy::SeekWptState);
	AddState("dashtoplayer", (statehandler)&ai_speedy::DashToPlayerState);
	AddState("dashtopoint", (statehandler)&ai_speedy::DashToPointState);
	AddState("dashtonewpoint", (statehandler)&ai_speedy::DashToNewPointState);

	// init entity, player and sbb
	setTEntity(new_entity);
	setPlayer(new_player);

	// transforms for the speedy and the player
	transform = ent->get<TCompTransform>();
	player_transform = player->get<TCompTransform>();

	// init wpts list
	for (int i = 0; i < 5; i++) {
		fixedWpts.push_back(VEC3((float)(rand() % 9), 0, (float)(rand() % 9)));
	}

	// current wpt
	curwpt = 0;

	// dash timer initialization
	dash_timer = dash_timer_reset;
	dash_ready = false;
	dash_target = VEC3(0, 0, 0);

	// reset the state
	ChangeState("idle");
}

void ai_speedy::IdleState() {
	ChangeState("nextwpt");
}

void ai_speedy::NextWptState()
{
	updateDashTimer();
	VEC3 front = transform->getFront();
	VEC3 target = fixedWpts[curwpt];

	bool aimed = aimToTarget(target);

	if (aimed) {
		ChangeState("seekwpt");
	}
}

void ai_speedy::SeekWptState() {
	updateDashTimer();
	float distance = squaredDistXZ(fixedWpts[curwpt], transform->getPosition());

	string next_action = decide_next_action();

	if (next_action == "dashtoplayer" && dash_ready) {
		dash_target = player_transform->getPosition();
		ChangeState(next_action);
	}
	else if (next_action == "dashtonewpoint" && dash_ready) {
		dash_target = VEC3(-(float)(rand() % 9), 0.0f, -(float)(rand() % 9));
		ChangeState(next_action);
	}
	else if (next_action == "dashtopoint" && dash_ready) {
		ChangeState(next_action);
	}
	else if (abs(distance) > 0.1f) {
		moveFront(speed);
	}
	else {
		transform->setPosition(fixedWpts[curwpt]);
		curwpt = (curwpt + 1) % fixedWpts.size();
		ChangeState("nextwpt");
	}
}

void ai_speedy::DashToPlayerState() {
	bool arrived = dashToTarget(dash_target);
	if (arrived) {
		resetDashTimer();
		ChangeState("nextwpt");
	}
}
void ai_speedy::DashToPointState() {
	bool arrived = dashToTarget(fixedWpts[curwpt]);
	if (arrived) {
		resetDashTimer();
		ChangeState("nextwpt");
	}
}

void ai_speedy::DashToNewPointState() {
	bool arrived = dashToTarget(dash_target);
	if (arrived) {
		resetDashTimer();
		ChangeState("nextwpt");
	}
}

string ai_speedy::decide_next_action() {
	int next_action = rand() % 100;

	if (next_action < dash_to_point_chance) {
		return "dashtopoint";
	}
	else if (next_action < dash_to_point_chance + dash_to_new_point_chance) {
		return "dashtonewpoint";
	}
	else if (next_action < dash_to_point_chance + dash_to_new_point_chance + dash_to_player_chance) {
		return "dashtoplayer";
	}
	else {
		return "seekwpt";
	}
}

bool ai_speedy::aimToTarget(VEC3 target) {
	float delta_yaw = transform->getDeltaYawToAimTo(target);

	if (abs(delta_yaw) > 0.001f) {
		float yaw, pitch;
		transform->getAngles(&yaw, &pitch);
		transform->setAngles(yaw + delta_yaw*0.005f, pitch);
		return false;
	}
	else {
		return true;
	}
}

void ai_speedy::moveFront(float movement_speed) {
	VEC3 front = transform->getFront();
	VEC3 position = transform->getPosition();

	transform->setPosition(VEC3(position.x + front.x*movement_speed, position.y, position.z + front.z*movement_speed));
}

bool ai_speedy::dashToTarget(VEC3 target) {
	bool aimed = aimToTarget(target);

	if (aimed) {
		moveFront(dash_speed);
	}

	float distance = squaredDistXZ(target, transform->getPosition());

	if (distance < 0.1f) {
		return true;
	}
	else {
		return false;
	}
}

void ai_speedy::updateDashTimer() {
	dash_timer -= getDeltaTime();
	if (dash_timer <= 0) {
		dash_ready = true;
	}
}

void ai_speedy::resetDashTimer() {
	dash_timer = dash_timer_reset;
	dash_ready = false;
}