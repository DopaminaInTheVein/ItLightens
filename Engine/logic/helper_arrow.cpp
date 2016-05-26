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
std:vector<CHandle> generators = SBB::readHandlesVector("generatorsHandles");
  if (generators.size() > 0) {
    CEntity * targete = target;
    TCompTransform * targett = targete->get<TCompTransform>();
    VEC3 targetpos = targett->getPosition();

    CEntity* e_owner = CHandle(this).getOwner();
    TCompTransform *ownT = e_owner->get<TCompTransform>();
    VEC3 ownpos = targetpos;
    ownpos.y += 1.8f;
    ownT->setPosition(ownpos);
    float distance = 99999999.999f;
    VEC3 nearGen;
    int i = 0;
    for (CHandle gen : generators) {
      CEntity * gene = gen;
      TCompTransform * gent = gene->get<TCompTransform>();
      VEC3 genpos = gent->getPosition();
      float d = squaredDist(targetpos, genpos);
      if (d < distance - 8.0f) {
        distance = d;
        nearGen = genpos;
      }
      ++i;
    }
    float yaw, pitch;
    ownT->getAngles(&yaw, &pitch);
    yaw += ownT->getDeltaYawToAimTo(nearGen);
    pitch += ownT->getDeltaPitchToAimTo(nearGen);
    ownT->setAngles(yaw, pitch);
  }
}