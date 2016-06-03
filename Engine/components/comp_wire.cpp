#include "mcv_platform.h"
#include "comp_wire.h"
#include "utils/XMLParser.h"
#include "handle\handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "entity_tags.h"

bool TCompWire::load(MKeyValue & atts)
{
	half_lenght = atts.getFloat("half_lenght", 1);
	direction = atts.getPoint("direction");
	return true;
}

void TCompWire::update(float dt)
{
	CEntity *p_e = player;
	TCompTransform *t_p = p_e->get<TCompTransform>();
	VEC3 player_position = t_p->getPosition();

	float d_squared = simpleDistXZ(org,player_position);

	if (d_squared < (half_lenght * 2)) {
		CanPass();
	}
}

void TCompWire::onCreate(const TMsgEntityCreated & msg)
{
	CHandle me_h = CHandle(this).getOwner();
	org = VEC3(0.0f, 0.0f, 0.0f);
	if (me_h.isValid()) {
		CEntity *me_e = me_h;
		if (me_e) {
			TCompTransform *t = me_e->get<TCompTransform>();
			if (t)
				org = t->getPosition();
		}
	}
}

void TCompWire::init()
{
	player = tags_manager.getFirstHavingTag(getID("raijin"));
}

void TCompWire::CanPass()
{
	TMsgWirePass msg;

	CEntity *p_e = player;
	TCompTransform *t_p = p_e->get<TCompTransform>();
	VEC3 player_position = t_p->getPosition();

	//TODO: calculate well direction, for now only z-axis
	if(player_position.z < org.z)
		msg.dst = org + direction*half_lenght;
	else
		msg.dst = org - direction*half_lenght;

	CEntity *player_e = player;
	player_e->sendMsg(msg);
}
