#include "mcv_platform.h"
#include "ai_speedy.h"
void ai_speedy::Init()
{
	// insert all states in the map
//	AddState("idle", (statehandler)&ai_speedy::IdleState);
	AddState("nextwpt", (statehandler)&ai_speedy::NextWptState);
	AddState("seekwpt", (statehandler)&ai_speedy::SeekWptState);
	AddState("dashtoplayer", (statehandler)&ai_speedy::DashToPlayerState);
	AddState("dashtopoint", (statehandler)&ai_speedy::DashToPointState);
	AddState("dashtonewpoint", (statehandler)&ai_speedy::DashToNewPointState);
	om = getHandleManager<ai_speedy>();
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();

	// transforms for the speedy and the player
	SetMyEntity();

	// current wpt
	curwpt = 0;

	// dash timer initialization
	dash_timer = (float)dash_timer_reset;
	dash_ready = false;
	dash_target = VEC3(0, 0, 0);

	// drop water timer initialization
	drop_water_timer = drop_water_timer_reset;
	drop_water_ready = false;

	// reset the state
	ChangeState("idle");
}

void ai_speedy::onSetPlayer(const TMsgSetPlayer& msg) {
	player = msg.player;
}

void ai_speedy::update(float elapsed) {
	// Update transforms
	SetMyEntity();
	transform = myEntity->get<TCompTransform>();
	CEntity * player_e = player;
	player_transform = player_e->get<TCompTransform>();
	// Recalc AI and update timers
	updateDashTimer();
	updateDropWaterTimer();
	Recalc();
}

// Loading the wpts
#define WPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d_%s", index, nameSufix);

bool ai_speedy::load(MKeyValue& atts) {
	int n = atts.getInt("wpts_size", 0);
	fixedWpts.resize(n);
	for (unsigned int i = 0; i < n; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		fixedWpts[i] = atts.getPoint(atrPos);
	}
	return true;
}

// Sets the entity
void ai_speedy::SetMyEntity() {
	myEntity = myParent;
}

// Speedy states

void ai_speedy::IdleState() {
	ChangeState("nextwpt");
}

void ai_speedy::NextWptState()
{
	VEC3 front = transform->getFront();
	VEC3 target = fixedWpts[curwpt];

	bool aimed = aimToTarget(target);

	if (aimed) {
		ChangeState("seekwpt");
	}
}

void ai_speedy::SeekWptState() 
{
	float distance = squaredDistXZ(fixedWpts[curwpt], transform->getPosition());

	string next_action = decide_next_action();

	if (next_action == "dashtoplayer" && dash_ready) {
		dash_target = player_transform->getPosition();
		float distance_to_player = squaredDistXZ(dash_target, transform->getPosition());
		if (abs(distance_to_player) <= max_dash_player_distance)
			ChangeState(next_action);
		else if (abs(distance) > 0.1f) 
			moveFront(speed);
	}
	else if (next_action == "dashtonewpoint" && dash_ready) {
		dash_target = VEC3(30.f, 0.0f, 0.0f);
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
		transform->setAngles(yaw + delta_yaw*rotation_speed*getDeltaTime(), pitch);
		return false;
	}
	else {
		return true;
	}
}

void ai_speedy::moveFront(float movement_speed) {
	VEC3 front = transform->getFront();
	VEC3 position = transform->getPosition();

	transform->setPosition(VEC3(position.x + front.x*movement_speed*getDeltaTime(), position.y, position.z + front.z*movement_speed*getDeltaTime()));
}

bool ai_speedy::dashToTarget(VEC3 target) {
	bool aimed = aimToTarget(target);

	if (aimed) {
		moveFront(dash_speed);
		if (drop_water_ready) {
			//CREATE WATER HERE
			resetDropWaterTimer();
		}
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
	dash_timer = (float)dash_timer_reset;
	dash_ready = false;
}

void ai_speedy::updateDropWaterTimer() {
	drop_water_timer -= getDeltaTime();
	if (drop_water_timer <= 0) {
		drop_water_ready = true;
	}
}

void ai_speedy::resetDropWaterTimer() {
	drop_water_timer = drop_water_timer_reset;
	drop_water_ready = false;
}