#include "mcv_platform.h"
#include "comp_aabb.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/draw_utils.h"
#include "entity.h"
#include "comp_transform.h"

bool TCompAABB::load(MKeyValue& atts) {
  Center = atts.getPoint("center");
  Extents = atts.getPoint("half_size");
  return true;
}

void TCompAABB::renderInMenu() {
  ImGui::InputFloat3("center", &Center.x);
  ImGui::InputFloat3("half_size", &Extents.x);
}

void TCompAABB::render() const {
  drawWiredAABB(*this, MAT44::Identity, VEC4(1, 0, 0, 1));
}

void TCompAABB::updateFromSiblingsLocalAABBs(CHandle h_entity) {
  // Time to update our AbsAABB based on the sibling components
  CEntity* e = h_entity;
  // Start by computing the local aabb
  e->sendMsg(TMsgGetLocalAABB{ this });
}

// Model * ( center +/- halfsize ) = model * center + model * half_size
AABB getRotatedBy(AABB src, const MAT44 &model) {
  AABB new_aabb;
  new_aabb.Center = VEC3::Transform(src.Center, model);
  new_aabb.Extents.x = src.Extents.x * fabsf(model(0, 0))
    + src.Extents.y * fabsf(model(1, 0))
    + src.Extents.z * fabsf(model(2, 0));
  new_aabb.Extents.y = src.Extents.x * fabsf(model(0, 1))
    + src.Extents.y * fabsf(model(1, 1))
    + src.Extents.z * fabsf(model(2, 1));
  new_aabb.Extents.z = src.Extents.x * fabsf(model(0, 2))
    + src.Extents.y * fabsf(model(1, 2))
    + src.Extents.z * fabsf(model(2, 2));
  return new_aabb;
}

// ------------------------------------------------------
void TCompAbsAABB::onCreate(const TMsgEntityCreated&) {
  updateFromSiblingsLocalAABBs(CHandle(this).getOwner());
  CEntity* e_owner = CHandle(this).getOwner();
  TCompTransform* c_trans = e_owner->get<TCompTransform>();
  AABB::Transform(*this, c_trans->asMatrix());
}

// Updates AbsAABB from LocalAABB and CompTransform
void TCompLocalAABB::updateAbs() {
  CEntity *e = CHandle(this).getOwner();
  assert(e);
  const TCompTransform *in_tmx = e->get< TCompTransform >();
  TCompAbsAABB *abs_aabb = e->get<TCompAbsAABB>();
  if (abs_aabb)
    *(AABB*)abs_aabb = getRotatedBy(*this, in_tmx->asMatrix());
}

void TCompLocalAABB::onCreate(const TMsgEntityCreated&) {
  updateFromSiblingsLocalAABBs(CHandle(this).getOwner());
}

void TCompLocalAABB::render() const {
  CEntity *e = CHandle(this).getOwner();
  const TCompTransform *in_tmx = e->get< TCompTransform >();
  drawWiredAABB(*this, in_tmx->asMatrix(), VEC4(1, 1, 0, 1));
}