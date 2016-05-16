#ifndef INC_COMPONENT_AABB_H_
#define INC_COMPONENT_AABB_H_

#include "geometry/geometry.h"
#include "components/comp_base.h"

struct TCompAABB : public AABB, public TCompBase {
  bool load(MKeyValue &atts);
  void renderInMenu();
  void render() const;
};

struct TCompAbsAABB : public TCompAABB {
};

// Updates AbsAABB from LocalAABB and CompTransform
struct TCompLocalAABB : public TCompAABB {
  void updateAbs();
  void render() const;
};

#endif
