#include "mcv_platform.h"
#include "comp_polarized.h"
#include "comp_transform.h"
#include "entity.h"
#include "handle\handle.h"
#include "comp_msgs.h"
#include "entity_tags.h"

void TCompPolarized::init()
{
	player_h = tags_manager.getFirstHavingTag(getID("target"));

	msg_in.origin = origin;
	msg_in.pol = pol;
	msg_in.range = true;

	msg_out.origin = origin;
	msg_out.pol = pol;
	msg_out.range = false;

}

void TCompPolarized::update(float elapsed)
{
	CEntity *e_p = player_h;
	if (e_p) {
		TCompTransform *t = e_p->get<TCompTransform>();
		VEC3 player_pos = t->getPosition();

		if (dist_effect_squared > squaredDist(player_pos, origin)) {
			if (!send) {
				send = true;
				sendMessagePlayer(msg_in);
			}
		}
		else {
			if (send) {
				send = false;
				sendMessagePlayer(msg_out);
			}
		}
	}
}

bool TCompPolarized::load(MKeyValue & atts)
{
	std::string read_s = atts.getString("pol", "neutral");

	if (read_s == "plus") {
		pol = PLUS;
	}
	else if (read_s == "minus") {
		pol = MINUS;
	}
	else {
		pol = NEUTRAL;		//default
	}
	
	return true;
}

void TCompPolarized::onCreate(const TMsgEntityCreated &)
{
	CHandle me_h = CHandle(this).getOwner();

	origin = VEC3(0.0f,0.0f,0.0f);
	if (me_h.isValid()) {
		CEntity *me_e = me_h;
		if (me_e) {
			TCompTransform *t = me_e->get<TCompTransform>();
			if (t) {
				origin = t->getPosition();
			}
		}
	}
}

void TCompPolarized::sendMessagePlayer(const TMsgPolarize & msg)
{
	CEntity * e = player_h;
	if (e) {
		e->sendMsg(msg);
	}
}
