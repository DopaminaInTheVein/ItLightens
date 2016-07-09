#include "mcv_platform.h"
#include "helper_arrow.h"
#include "components/comp_msgs.h"
#include "sbb.h"

bool LogicHelperArrow::load(MKeyValue& atts)
{
  return true;
}

void LogicHelperArrow::onSetTarget(const TMsgSetTarget & tasr)
{
  target = tasr.target;
}

void LogicHelperArrow::update(float elapsed) {
	PROFILE_FUNCTION("helper arrow: update");
  std::vector<CHandle> generators = SBB::readHandlesVector("generatorsHandles");
  CEntity * targete = target;
  TCompTransform * targett = targete->get<TCompTransform>();
  TCompLife * targetl = targete->get<TCompLife>();
  VEC3 targetpos = targett->getPosition();

  CEntity* e_owner = CHandle(this).getOwner();
  TCompTransform *ownT = e_owner->get<TCompTransform>();
  VEC3 ownpos = targetpos;
  ownpos.y += 1.8f;
  float distance = 99999999.999f;
  VEC3 nearGen;
  for (CHandle gen : generators) {
    CEntity * gene = gen;
    TCompTransform * gent = gene->get<TCompTransform>();
    VEC3 genpos = gent->getPosition();
    float d = squaredDist(targetpos, genpos);
    if (d < distance - 8.0f) {
      distance = d;
      nearGen = genpos;
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