#include "mcv_platform.h"
#include "slb_public_functions.h"
#include "handle\handle_manager.h"
#include "components\entity.h"
#include "components\entity_parser.h"
#include "components\comp_charactercontroller.h"

using namespace IdEntities;

void SLBPlayer::getPlayer() {
	VHandles targets = tags_manager.getHandlesByTag(getID("player"));
	CHandle thePlayer = targets[targets.size() - 1];
	player_handle = thePlayer;
}

// player functions
void SLBPlayer::setPlayerPosition(float x, float y, float z) {
	getPlayer();
	const PxVec3 new_position(x, y, z);

	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	entity_controller->teleport(new_position);
}

float SLBPlayer::getPlayerX() {
	getPlayer();
	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().x;
}

float SLBPlayer::getPlayerY() {
	getPlayer();
	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().y;
}

float SLBPlayer::getPlayerZ() {
	getPlayer();
	CEntity* entity = player_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().z;
}

// generic handle function
void SLBHandle::getHandleById(int id) {	
	CHandle handle = IdEntities::findById(id);
	real_handle = handle;
}

void SLBHandle::getHandleByNameTag(const char* name, const char* tag) {
	VHandles targets = tags_manager.getHandlesByTag(getID(tag));
	CHandle handle = findByName(targets, name);
	real_handle = handle;
}

void SLBHandle::setPosition(float x, float y, float z) {
	const PxVec3 new_position(x, y, z);

	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	entity_controller->teleport(new_position);
}

float SLBHandle::getX() {
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().x;
}

float SLBHandle::getY() {
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().y;
}

float SLBHandle::getZ() {
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().z;
}

// public functions
void SLBPublicFunctions::execCommand(const char* exec_code, float exec_time) {
	// create the new command
	command new_command;
	new_command.code = exec_code;
	new_command.execution_time = exec_time;
	// add the new command to the queue
	logic_manager->getCommandQueue()->push_back(new_command);
}

void SLBPublicFunctions::print(const char* to_print) {
	dbg(to_print);
}

