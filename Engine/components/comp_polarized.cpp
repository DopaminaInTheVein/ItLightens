#include "mcv_platform.h"
#include "comp_polarized.h"
#include "comp_transform.h"
#include "entity.h"
#include "handle\handle.h"
#include "comp_msgs.h"
#include "entity_tags.h"
#include "comp_physics.h"
#include "comp_charactercontroller.h"

void TCompPolarized::init()
{
	player_h = tags_manager.getFirstHavingTag(getID("player"));

	CHandle e_h = CHandle(this).getOwner();
	CEntity * e = e_h;
	TCompTransform *t = e->get<TCompTransform>();
	last_position = t->getPosition();

	msg_in.handle = e_h;
	msg_in.range = true;

	msg_out.handle = e_h;
	msg_out.range = false;
}

bool TCompPolarized::getUpdateInfo() {
	GET_COMP(t, CHandle(this).getOwner(), TCompTransform);
	if (!t) return false;
	origin = t->getPosition() + force.offset;
	//Debug->DrawLine(t->getPosition(), t->getPosition() + force.offset);
}

void TCompPolarized::update(float elapsed)
{
	//if (DEBUG_CODE) {
	//	CHandle me_h = CHandle(this).getOwner();

	//	origin = VEC3(0.0f, 0.0f, 0.0f);
	//	CEntity *me_e = me_h;
	//	TCompTransform *t = me_e->get<TCompTransform>();
	//	if (t) {
	//		auto my_position = t->getPosition();
	//		origin = my_position + force.offset;
	//		Debug->DrawLine(my_position, origin);
	//	}
	//}

	CEntity *e_p = player_h;
	if (e_p) {
		TCompCharacterController *cc = e_p->get<TCompCharacterController>();
		assert(cc || fatal("Player doesnt have character controller!"));
		VEC3 player_pos = cc->GetPosition();

		if (mType == FIXED) {
			force.deltaPos = origin - player_pos;
			force.distance = simpleDist(origin, player_pos);
			//Debug->DrawLine(origin, player_pos);
			if (dist_effect_fixed > force.distance) {
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
		else if (mType == FREE) {
			if (mPlayer_state != NEUTRAL) {
				CHandle e_h = CHandle(this).getOwner();
				CEntity *e = e_h;

				if (e_h.isValid()) {		//update real position, object can be moved
					CEntity *me_e = e_h;
					if (me_e) {
						TCompTransform *t = me_e->get<TCompTransform>();
						if (t) {
							origin = t->getPosition();
							if (origin != last_position) {
								moving = true;
								last_position = origin;
							}
							else {
								moving = false;
							}
						}
					}
				}

				float dist = simpleDist(player_pos, origin);
				if (dist_effect_free > dist) {
					VEC3 direction = player_pos - origin;
					TCompPhysics *p = e->get<TCompPhysics>();

					if (p) {
						float forceMin = 3.f;
						float forceMax = 30.f;
						float forceVal = 0.f;
						float distRepulsion = 9.f;
						float distAttraction = 9.f;

						if (force.polarity == mPlayer_state) {
							//Repulsion
							forceVal = forceMax - (forceMax - forceMin) * clamp((dist / distRepulsion), 0, 1);
							p->AddForce(-direction * forceVal);
						}
						else {
							static float forceStop = 0.f;
							//Attraction
							if (dist > dist_near) {
								if (dist > distRepulsion) forceVal = forceMin;
								else forceVal = 15.f;
								forceStop = forceVal / 2;
							}
							else {
								forceVal = -forceStop;
								forceStop = forceStop - (0.99f * forceStop) * elapsed;
							}
							p->AddForce(direction * forceVal);
						}
					}
				}
			}
		}
	}
}

bool TCompPolarized::load(MKeyValue & atts)
{
	//Polarity
	std::string read_s = atts.getString("pol", "neutral");
	if (read_s == "plus") {
		force.polarity = PLUS;
	}
	else if (read_s == "minus") {
		force.polarity = MINUS;
	}
	else {
		force.polarity = NEUTRAL;		//default
	}

	//Area
	force.offset = atts.getPoint("offset");
	if (force.offset == VEC3(0.f, 0.f, 0.f)) {
		force.offset = VEC3(0.f, 0.5f, 0.f);
	}

	return true;
}

bool TCompPolarized::save(std::ofstream& os, MKeyValue& atts)
{
	std::string polarities[] = { "neutral", "minus", "plus" };
	atts.put("pol", polarities[force.polarity]);
	if (!isZero(force.offset)) atts.put("offset", force.offset);
	return true;
}

void TCompPolarized::onCreate(const TMsgEntityCreated &)
{
	CHandle me_h = CHandle(this).getOwner();
	CEntity* e = me_h;
	dbg("on create polarized [%s]\n", e->getName());

	origin = VEC3(0.0f, 0.0f, 0.0f);
	if (me_h.isValid()) {
		CEntity *me_e = me_h;
		if (me_e) {
			// Set FIXED or FREE
			TCompPhysics * p = me_e->get<TCompPhysics>();
			if (p) {
				if (p->GetMass() > mThresholdMass) mType = FIXED;
				else if (p->isKinematic()) mType = FIXED;
				else mType = FREE;
			}
			else {
				//Not physic component --> FIXED
				mType = FIXED;
			}

			//Origin and area
			TCompTransform *t = me_e->get<TCompTransform>();
			if (t) {
				//Set origin (remains at the moment for FREE behaviour)
				auto my_position = t->getPosition();
				origin = my_position + force.offset;
			}
		}
	}
}

void TCompPolarized::onPolarize(const TMsgPlayerPolarize & msg)
{
	mPlayer_state = msg.type;
}

void TCompPolarized::sendMessagePlayer(const TMsgPolarize & msg)
{
	CEntity * e = player_h;
	if (e) {
		e->sendMsg(msg);
	}
}

PolarityForce TCompPolarized::getForce() {
	if (enabled) return force;
	return PolarityForce();
}

void TCompPolarized::setEnabled(bool new_enabled)
{
	enabled = new_enabled;
}