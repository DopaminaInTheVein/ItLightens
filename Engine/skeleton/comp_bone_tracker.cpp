#include "mcv_platform.h"
#include "comp_bone_tracker.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_transform.h"
#include "skeleton/comp_skeleton.h"
#include "cal3d/cal3d.h"


extern VEC3 Cal2Engine(CalVector v);
extern CQuaternion Cal2Engine(CalQuaternion q);

bool TCompBoneTracker::load(MKeyValue& atts) {
  strcpy(bone_name, atts.getString("bone", "").c_str());
  strcpy(entity_name, atts.getString("entity", "").c_str());
  bone_id = -1;
  h_entity = CHandle();
  return true;
}

void TCompBoneTracker::onGroupCreated(const TMsgEntityGroupCreated& msg) {
  h_entity = findByName(*msg.handles, entity_name);
  CEntity* e = h_entity;
  if (!e)
    return;
  TCompSkeleton* skel = e->get<TCompSkeleton>();
  if (!skel)
    return;
  bone_id = skel->model->getSkeleton()->getCoreSkeleton()->getCoreBoneId(bone_name);
}

void TCompBoneTracker::renderInMenu() {
  bool changed = false;
  changed |= ImGui::InputText("Bone Name", bone_name, sizeof(bone_name));
  if (bone_id != -1)
    ImGui::Text("Bone %d", bone_id);
  h_entity.renderInMenu();
}

void TCompBoneTracker::update(float dt) {
  CEntity* e = h_entity;
  if (!e)
    return;
  TCompSkeleton* skel = e->get<TCompSkeleton>();
  if (!skel || bone_id == -1)
    return;
  auto bone = skel->model->getSkeleton()->getBone(bone_id);
  auto rot = Cal2Engine(bone->getRotationAbsolute());
  auto trans = Cal2Engine(bone->getTranslationAbsolute());
  
  CEntity* my_e = CHandle(this).getOwner();
  TCompTransform* tmx = my_e->get<TCompTransform>();
  assert(tmx);
  tmx->setPosition(trans);
  tmx->setRotation(rot);
}

