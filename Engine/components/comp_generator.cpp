#include "mcv_platform.h"
#include "comp_generator.h"
#include "utils/XMLParser.h"
#include "handle\handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"

bool TCompGenerator::load(MKeyValue & atts)
{
	rad = atts.getFloat("rad", 1);
	return true;
}

void TCompGenerator::mUpdate(float dt)
{
	//nothing to do
}

void TCompGenerator::onTriggerEnter(const TMsgTriggerIn & msg)
{
	
	CHandle h_in = msg.other;
	if (h_in.hasTag("player")) {
		CanRec(true);
	}
}

void TCompGenerator::onTriggerExit(const TMsgTriggerOut & msg)
{
	
	CHandle h_in = msg.other;
	if (h_in.hasTag("player")) {
		CanRec(false);
	}
}

void TCompGenerator::onCreate(const TMsgEntityCreated & msg)
{
	CHandle me_h = CHandle(this).getOwner();
	CEntity *me_e = me_h;
	TCompTransform *t = me_e->get<TCompTransform>();

	org = t->getPosition();
}

void TCompGenerator::init()
{
	player = tags_manager.getFirstHavingTag(getID("player"));
}

void TCompGenerator::CanRec(bool new_range)
{
	TMsgCanRec msg;
	CEntity *p_e = player;
	TCompTransform *t_p = p_e->get<TCompTransform>();
	VEC3 player_position = t_p->getPosition();

	msg.range = new_range;
	CEntity *player_e = player;
	player_e->sendMsg(msg);
}
