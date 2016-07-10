#include "mcv_platform.h"
#include "comp_room_switch.h"
#include "comp_room.h"
#include "utils/XMLParser.h"
#include "logic/sbb.h"
#include "handle\handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"

bool TCompRoomSwitch::load(MKeyValue & atts)
{
	room_back = atts.getInt("back_room", -1);
	room_front = atts.getInt("front_room", -1);
	return true;
}

void TCompRoomSwitch::mUpdate(float dt)
{
	//nothing to do
}

void TCompRoomSwitch::onTriggerExit(const TMsgTriggerOut & msg)
{
	CHandle h_out = msg.other;
	if (!h_out.hasTag("player")) { return; }
	CEntity * pe = h_out;
	TCompRoom * room = pe->get<TCompRoom>();
	TCompTransform * pt = pe->get<TCompTransform>();

	CHandle me_h = CHandle(this).getOwner();
	CEntity * me_e = me_h;
	TCompTransform * t = me_e->get<TCompTransform>();
	if (t->isInFront(pt->getPosition())) {
		if (room->setName(room_front)) {
			SBB::postSala(room_front);
		}
	}
	else {
		if (room->setName(room_back)) {
			SBB::postSala(room_back);
		}
	}
}

void TCompRoomSwitch::onCreate(const TMsgEntityCreated & msg)
{
}

void TCompRoomSwitch::init()
{
}