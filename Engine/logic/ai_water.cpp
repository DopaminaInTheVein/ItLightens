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

	AddState("idle", (statehandler)&water_controller::Idle);
	AddState("die", (statehandler)&water_controller::Die);

	SetHandleMeInit();
	player = tags_manager.getFirstHavingTag(getID("target"));

	ChangeState("idle");
}

void water_controller::update(float elapsed) {
	updateTTL();
	Recalc();
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
	getHandleManager<water_controller>()->destroyHandle(myHandle);
}

void water_controller::updateTTL() {
	ttl -= getDeltaTime();
	if (ttl <= 0) {
		dead = true;
	}
}

void water_controller::tryToDamagePlayer() {

	SetMyEntity();

	CEntity* e_player = player;
	TCompTransform* player_transform = e_player->get<TCompTransform>();
	VEC3 player_position = player_transform->getPosition();
	TCompTransform* water_transform = myEntity->get<TCompTransform>();
	VEC3 water_position = water_transform->getPosition();

	float distance = squaredDistXZ(water_position, player_position);

	if (distance < 1.f) {
		TMsgDamage dmg;
		dmg.source = water_position;
		dmg.sender = myParent;
		dmg.points = speedy_water_damage * getDeltaTime();
		dmg.dmgType = SPEEDY_WATER;
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
