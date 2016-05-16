#include "mcv_platform.h"
#include "comp_culling.h"
#include "comp_aabb.h"

void TCompCulling::renderInMenu() {

}

void TCompCulling::update(MAT44 view_proj) {

  // Construir el set de planos usando la view_proj
  planes.fromViewProjection(view_proj);

  // Start from zero
  bits.reset();

  // Traverse all aabb's defined in the game
  // and test them
  //auto hm = getHandleManager<TCompAbsAABB>();
  //hm->each([this](TCompAbsAABB* aabb) {
  //  // test the 6 planes with the aabb
  //  if (planes.testIntersect(aabb))
  //    bits.set(idx);
  //})



}
