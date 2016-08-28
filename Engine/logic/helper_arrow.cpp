#include "mcv_platform.h"
#include "helper_arrow.h"
#include "components/comp_msgs.h"
#include "sbb.h"

#include "components/comp_generator.h"

bool LogicHelperArrow::load(MKeyValue& atts)
{
	return true;
}

void LogicHelperArrow::init()
{
	CHandle player = tags_manager.getFirstHavingTag("player");
	TMsgSetTarget msg;
	msg.target = player;
	msg.who = PLAYER;
	CHandle(this).getOwner().sendMsg(msg);
}

void LogicHelperArrow::update(float elapsed) {
	PROFILE_FUNCTION("helper arrow: update");
	CEntity * targeteRaigin = tags_manager.getFirstHavingTag(getID("raijin"));
	CHandle target = tags_manager.getFirstHavingTag(getID("player"));
	CEntity * targete = target;
	if (!targete) return;
	TCompTransform * targett = targete->get<TCompTransform>();
	if (!targett) return;
	TCompLife * targetl = targeteRaigin->get<TCompLife>();
	if (!targetl) return;
	VEC3 targetpos = targett->getPosition();

	CEntity* e_owner = CHandle(this).getOwner();
	if (!e_owner) return;

	TCompTransform *ownT = e_owner->get<TCompTransform>();
	if (!ownT) return;

	VEC3 ownpos = targetpos;
	ownpos.y += 1.8f;
	if (!target.hasTag("raijin")) {
		ownpos.y += 1.8f;
	}
	float distance = 99999999.999f;
	VEC3 nearGen;
	for (CHandle gen : TCompGenerator::all_generators) {
		GET_COMP(g, gen, TCompGenerator);
		if (g  && g->isUsable()) {
			CEntity * gene = gen;
			if (!gene) return;
			TCompTransform * gent = gene->get<TCompTransform>();
			if (!gent) return;
			VEC3 genpos = gent->getPosition();
			float d = squaredDist(targetpos, genpos);
			if (d < distance - 8.0f) {
				distance = d;
				nearGen = genpos;
			}
		}
	}

	// Es mostra si estem a prop amb vida justa o si tenim poquissima vida
	bool aprop = distance < 900.0f;
	bool pocavida = targetl->currentlife < 50.0f;
	bool moltpocavida = targetl->currentlife < 15.0f;

	if (moltpocavida || pocavida && aprop) {
		float yaw, pitch;
		ownT->getAngles(&yaw, &pitch);
		yaw += ownT->getDeltaYawToAimTo(nearGen);
		pitch += ownT->getDeltaPitchToAimTo(nearGen);

		ownT->setPosition(ownpos);
		ownT->setAngles(yaw, pitch);
	}
	else {
		ownpos.y = 100000.0f;
		ownT->setPosition(ownpos);
	}
}