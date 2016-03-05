#include "mcv_platform.h"
#include "ai_water.h"

#include <windows.h>
#include "handle\object_manager.h"
#include "components\comp_transform.h"
#include "components\entity.h"

#include "components\entity_tags.h"

int water_controller::id_curr_max_waters = 0;

void water_controller::Init() {
	om = getHandleManager<water_controller>();	//list handle water in game

	id_water = ++water_controller::id_curr_max_waters;
	full_name = "water_" + to_string(id_water);
	dbg("Init Agua!! %i\n", id_water);
	AddState("idle", (statehandler)&water_controller::Idle);
	AddState("die", (statehandler)&water_controller::Die);
	AddState("dead", (statehandler)&water_controller::Dead);

	SetHandleMeInit();
	player = tags_manager.getFirstHavingTag(getID("target"));

	ChangeState("idle");
}

void water_controller::update(float elapsed) {
	Recalc();
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
	dbg("Muerte Agua!! %i\n", id_water);
	if (myParent.isValid())
		getHandleManager<CEntity>()->destroyHandle(myParent);
	ChangeState("dead");
}

void water_controller::Dead()
{
	//Do nothing
}

void water_controller::updateTTL() {

	if (water_type != PERMANENT) {
		/********************/
		TCompTransform* water_transform = myEntity->get<TCompTransform>();
		VEC3 water_position = water_transform->getPosition();
		dbg("Posicion Agua!! %i: %f - %f - %f\n", id_water, water_position.x, water_position.y, water_position.z);
		/********************/
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

	if (distance < 1.f) {
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
	ImGui::Text("Node : %s", out[SBB::readInt(full_name)].c_str());
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
