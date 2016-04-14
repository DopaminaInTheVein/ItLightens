#include "mcv_platform.h"
#include "slb_public_functions.h"
#include "components\entity.h"
#include "components\comp_charactercontroller.h"

void SLBHandle::getPlayer() {
	VHandles targets = tags_manager.getHandlesByTag(getID("player"));
	CHandle thePlayer = targets[targets.size() - 1];
	real_handle = thePlayer;
}

// generic functions
void SLBHandle::setPlayerPosition(float x, float y, float z) {
	getPlayer();
	const PxVec3 new_position(x, y, z);

	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	entity_controller->teleport(new_position);
}

float SLBHandle::getPlayerX() {
	getPlayer();
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().x;
}

float SLBHandle::getPlayerY() {
	getPlayer();
	CEntity* entity = real_handle;
	TCompCharacterController* entity_controller = entity->get<TCompCharacterController>();

	return entity_controller->GetPosition().y;
}

float SLBHandle::getPlayerZ() {
	getPlayer();
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

