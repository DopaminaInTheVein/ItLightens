#include "mcv_platform.h"
#include "workbench.h"
#include "components/comp_msgs.h"
#include "logic/sbb.h"

void workbench::onCreate(const TMsgEntityCreated&) {
	GET_COMP(transform, ClHandle(this).getOwner(), TCompTransform);
	SBB::addVEC3ToVector("wb_pos", transform->getPosition());
	dbg("Hello workbench\n");
}