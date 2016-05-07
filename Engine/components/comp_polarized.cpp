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

void TCompPolarized::update(float elapsed)
{
	updatePxTransform();
	CEntity *e_p = player_h;
	if (e_p) {
		TCompCharacterController *cc = e_p->get<TCompCharacterController>();
		assert(cc || fatal("Player doesnt have character controller!"));
		VEC3 player_pos = cc->GetPosition();

		if (mType == FIXED) {
			PxVec3* pxClosestPoint = new PxVec3();
			PxVec3 pxPlayerPos = PhysxConversion::Vec3ToPxVec3(player_pos);
			force.distance = PxGeometryQuery::pointDistance(pxPlayerPos, *m_area, m_transform, pxClosestPoint);
			if (force.distance > 0.f) {
				VEC3 closestPoint = PhysxConversion::PxVec3ToVec3(*pxClosestPoint);
				force.deltaPos = closestPoint - player_pos;
				//force.deltaPos = origin - player_pos;
				assert(isNormal(force.deltaPos));
			} else {
				force.deltaPos = origin - player_pos;
			}
			
			if (dist_effect_squared_fixed > force.distance) {
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


				float dist = squaredDist(player_pos, origin);
				if (dist_effect_squared_free > dist) {

					VEC3 direction = player_pos - origin;
					TCompPhysics *p = e->get<TCompPhysics>();
					
					if (p) {
						if (force.polarity != mPlayer_state)	p->AddForce((-direction*100)/dist);		//opposite pols
						else {
							if(dist_near < dist)
								p->AddForce((direction * 10)); 
						}
					}
				}
			}
		}
	}
}

void TCompPolarized::updatePxTransform()
{
	CEntity* eMe = CHandle(this).getOwner();
	if (eMe) {
		TCompTransform* t = eMe->get<TCompTransform>();
		float movement = simpleDist(t->getPosition(), last_position);
		if (movement > mEpsilonMove) {
			m_transform = PhysxConversion::ToPxTransform(t->getPosition(), t->getRotation());
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
	const VEC3 size = atts.getPoint("size");
	if (size.x != 0 || size.y != 0 | size.z != 0) {
		PxVec3 pxSize = PhysxConversion::Vec3ToPxVec3(size);
		m_area = new PxBoxGeometry();
		g_PhysxManager->CreateBoxGeometry(pxSize, *m_area);
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
			// Set FIXED or FREE
			TCompPhysics * p = me_e->get<TCompPhysics>();
			if (p) {
				if (p->GetMass() > mThresholdMass) mType = FIXED;
				if (p->isKinematic()) mType = FIXED;
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
				origin = t->getPosition();

				//Set pxTransform (for FIXED behaviour)
				if (m_area) {
					m_transform = PhysxConversion::ToPxTransform(
						t->getPosition(),
						t->getRotation()
					);
				}
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
	return force;
}



