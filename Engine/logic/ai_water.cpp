#include "mcv_platform.h"
#include "ai_water.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"

#include "components\entity_tags.h"

int water_controller::id_curr_max_waters = 0;

map<string, statehandler> water_controller::statemap = {};
map<int, string> water_controller::out = {};

void water_controller::readIniFileAttr() {
	CHandle h = CHandle(this).getOwner();
	if (h.isValid()) {
		if (h.hasTag("water")) {
			CApp &app = CApp::get();
			std::string file_ini = app.file_initAttr_json;
			map<std::string, float> fields = readIniAtrData(file_ini, "ai_water");

			assignValueToVar(permanent_water_damage, fields);
			assignValueToVar(dropped_water_damage, fields);
			assignValueToVar(permanent_max_ttl, fields);
			assignValueToVar(dropped_max_ttl, fields);
			assignValueToVar(damage_radius, fields);
		}
	}
}

void water_controller::Init() {
	//read attributes from file
	readIniFileAttr();

	om = getHandleManager<water_controller>();	//list handle water in game

	id_water = ++water_controller::id_curr_max_waters;
	full_name = "water_" + to_string(id_water);

	if (statemap.empty()) {
		AddState("idle", (statehandler)&water_controller::Idle);
		AddState("die", (statehandler)&water_controller::Die);
		AddState("dead", (statehandler)&water_controller::Dead);
	}

	SetHandleMeInit();
	player = tags_manager.getFirstHavingTag(getID("raijin"));

	ChangeState("idle");
}

void water_controller::update(float elapsed) {
	if (myHandle.isValid())
		Recalc();
	if (myParent.isValid())
		updateTTL();
}

void water_controller::onCreate(const TMsgEntityCreated& msg) {
	Init();
}

void water_controller::Idle()
{
	if (dead)
		ChangeState("die");
	else
		tryToDamagePlayer();
}

void water_controller::Die()
{
	myParent.destroy();
	ChangeState("dead");
}

void water_controller::Dead()
{
	//Do nothing
}

void water_controller::updateTTL() {
	if (water_type != PERMANENT) {
		ttl -= getDeltaTime();
		if (ttl <= 0.0) {
			dead = true;
		}
	}
}

void water_controller::tryToDamagePlayer() {
	SetMyEntity();
	if (!myEntity) return;
	CEntity* e_player = player;
	TCompTransform* player_transform = e_player->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	TCompTransform* water_transform = myEntity->get<TCompTransform>();
	VEC3 water_position = water_transform->getPosition();

	float distance = squaredDist(water_position, player_position);

	if (distance < damage_radius && player_position.y - water_position.y < 0.5 && !sendMsgDamage) {
		TMsgDamage dmg;
		sendMsgDamage = !sendMsgDamage;
		dmg.modif = 5.0f;
		e_player->sendMsg(dmg);
	}
	else if ((distance > damage_radius || player_position.y - water_position.y > 0.5) && sendMsgDamage) {
		TMsgStopDamage dmg;
		sendMsgDamage = !sendMsgDamage;
		e_player->sendMsg(dmg);
	}
}

void water_controller::renderInMenu()
{
	//ImGui::Text("Node : %s", out[SBB::readInt(full_name)].c_str());
	ImGui::Text("timer : %.4f", ttl);
}

void water_controller::SetHandleMeInit()
{
	myHandle = om->getHandleFromObjAddr(this);
	myParent = myHandle.getOwner();
}

void water_controller::SetMyEntity() {
	myEntity = myParent;
}

bool water_controller::load(MKeyValue& atts) {
	string type = atts.getString("type", "permanent");
	if (type == "permanent") {
		water_type = PERMANENT;
	}
	else if (type == "dropped") {
		water_type = DROPPED;
	}
	else {
		fatal("Wrong Type Water!");
	}

	switch (water_type) {
	case PERMANENT:
		damage = permanent_water_damage;
		ttl = permanent_max_ttl;
		break;
	case DROPPED:
		damage = dropped_water_damage;
		ttl = dropped_max_ttl;
		break;
	default:
		damage = permanent_water_damage;
		ttl = permanent_max_ttl;
	}

	return true;
}