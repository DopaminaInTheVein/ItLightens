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

void TCompGenerator::update(float dt)
{
	player = player = tags_manager.getFirstHavingTag(getID("target"));
	CEntity *p_e = player;
	TCompTransform *t_p = p_e->get<TCompTransform>();
	VEC3 player_position = t_p->getPosition();

	float d_squared = simpleDistXZ(org, player_position);

	if (d_squared < rad) {
		CanRec();
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
	player = tags_manager.getFirstHavingTag(getID("target"));
}

void TCompGenerator::CanRec()
{
	TMsgCanRec msg;
	CEntity *p_e = player;
	TCompTransform *t_p = p_e->get<TCompTransform>();
	VEC3 player_position = t_p->getPosition();


	CEntity *player_e = player;
	player_e->sendMsg(msg);
}
