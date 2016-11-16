#include "mcv_platform.h"
#include "comp_room_switch.h"
#include "comp_room.h"
#include "utils/XMLParser.h"
#include "logic/sbb.h"
#include "handle\handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_tasklist.h"
#include "entity_tags.h"
#include "player_controllers/player_controller_base.h"

bool TCompRoomSwitch::load(MKeyValue & atts)
{
	room_back = TRoom(atts.getInt("back_room", -1));
	room_front = TRoom(atts.getInt("front_room", -1));
	return true;
}

bool TCompRoomSwitch::save(std::ofstream& ofs, MKeyValue& atts)
{
	atts.put("back_room", room_back.print());
	atts.put("front_room", room_front.print());
	return true;
}

void TCompRoomSwitch::mUpdate(float dt)
{
	//nothing to do
}

void TCompRoomSwitch::onTriggerExit(const TMsgTriggerOut & msg)
{
	CHandle h_out = msg.other;
	if (h_out != CPlayerBase::handle_player) { return; }
	CEntity * pe = h_out;
	TCompRoom * room = pe->get<TCompRoom>();
	TCompTransform * pt = pe->get<TCompTransform>();

	CHandle me_h = CHandle(this).getOwner();
	CEntity * me_e = me_h;
	TCompTransform * t = me_e->get<TCompTransform>();
	if (t->isInFront(pt->getPosition())) {
		room->setRoom(room_front);
		SBB::postSala(room_front.getSingleRoom());
	}
	else {
		room->setRoom(room_back);
		SBB::postSala(room_back.getSingleRoom());
	}
	CEntity * tasklist_e = tags_manager.getFirstHavingTag(getID("tasklist"));
	if (tasklist_e) {
		for (int i = 0; i < TASKLIST_PICKUP_PILA; ++i) {
			Tasklist * tasklist_comp = tasklist_e->get<Tasklist>();
			tasklist_comp->completeTask(i);
		}
	}
	CApp::get().init_render_scene();
}

void TCompRoomSwitch::onCreate(const TMsgEntityCreated & msg)
{
}

void TCompRoomSwitch::init()
{
}