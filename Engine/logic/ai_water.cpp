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

void water_controller::Init() {
	om = getHandleManager<water_controller>();	//list handle water in game

	id_water = ++water_controller::id_curr_max_waters;
	full_name = "water_" + to_string(id_water);

	if (statemap.empty()) {
		AddState("idle", (statehandler)&water_controller::Idle);
		AddState("die", (statehandler)&water_controller::Die);
		AddState("dead", (statehandler)&water_controller::Dead);
	}

	SetHandleMeInit();
	player = tags_manager.getFirstHavingTag(getID("player"));

	ChangeState("idle");
}

void water_controller::update(float elapsed) {
	
	if(myHandle.isValid()) 
		Recalc();
	if(myParent.isValid()) 
		updateTTL();
			
}

void water_controller::onSetWaterType(const TMsgSetWaterType& msg) {
	switch (msg.type) {
	case PERMANENT:
		water_type = PERMANENT;
		damage = permanent_water_damage;
		ttl = permanent_max_ttl;
		break;
	case DROPPED:
		water_type = DROPPED;
		damage = dropped_water_damage;
		ttl = dropped_max_ttl;
		break;
	default:
		water_type = PERMANENT;
		damage = permanent_water_damage;
		ttl = permanent_max_ttl;
	}
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

	CEntity* e_player = player;
	TCompTransform* player_transform = e_player->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	TCompTransform* water_transform = myEntity->get<TCompTransform>();
	VEC3 water_position = water_transform->getPosition();

	float distance = squaredDist(water_position, player_position);

	if (distance < damage_radius && player_position.y - water_position.y < 0.5) {
		TMsgDamage dmg;
		dmg.source = water_position;
		dmg.sender = myParent;
		dmg.points = damage * getDeltaTime();
		dmg.dmgType = WATER;
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
