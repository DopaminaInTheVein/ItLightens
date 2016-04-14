#include "mcv_platform.h"
#include "slb_public_functions.h"
#include "components\entity.h"
#include "components\comp_transform.h"

void SLBHandle::getPlayer() {
	VHandles targets = tags_manager.getHandlesByTag(getID("player"));
	CHandle thePlayer = targets[targets.size() - 1];
	real_handle = thePlayer;
}

// generic functions
void SLBHandle::setPosition(float x, float y, float z) {
	VEC3 new_position(x, y, z);

	CEntity* entity = real_handle;
	TCompTransform* entity_transform = entity->get<TCompTransform>();

	entity_transform->setPosition(new_position);
}

float SLBHandle::getX() {
	CEntity* entity = real_handle;
	TCompTransform* entity_transform = entity->get<TCompTransform>();

	return entity_transform->getPosition().x;
}

float SLBHandle::getY() {
	CEntity* entity = real_handle;
	TCompTransform* entity_transform = entity->get<TCompTransform>();

	return entity_transform->getPosition().y;
}

float SLBHandle::getZ() {
	CEntity* entity = real_handle;
	TCompTransform* entity_transform = entity->get<TCompTransform>();

	return entity_transform->getPosition().z;
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

