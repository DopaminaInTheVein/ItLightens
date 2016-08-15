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
	room_back.push_back(atts.getInt("back_room", -1));
	room_front.push_back(atts.getInt("front_room", -1));
	return true;
}

bool TCompRoomSwitch::save(std::ofstream& ofs, MKeyValue& atts)
{
	atts.put("back_room", room_back[0]);
	atts.put("front_room", room_front[0]);
	return true;
}

void TCompRoomSwitch::mUpdate(float dt)
{
	//nothing to do
}

void TCompRoomSwitch::onTriggerExit(const TMsgTriggerOut & msg)
{
	ClHandle h_out = msg.other;
	if (!h_out.hasTag("player")) { return; }
	CEntity * pe = h_out;
	TCompRoom * room = pe->get<TCompRoom>();
	TCompTransform * pt = pe->get<TCompTransform>();

	ClHandle me_h = ClHandle(this).getOwner();
	CEntity * me_e = me_h;
	TCompTransform * t = me_e->get<TCompTransform>();
	if (t->isInFront(pt->getPosition())) {
		if (room->setName(room_front)) {
			SBB::postSala(room_front[0]);
		}
	}
	else {
		if (room->setName(room_back)) {
			SBB::postSala(room_back[0]);
		}
	}
}

void TCompRoomSwitch::onCreate(const TMsgEntityCreated & msg)
{
}

void TCompRoomSwitch::init()
{
}