#ifndef INC_COMPONENT_AABB_H_
#define INC_COMPONENT_AABB_H_

#include "geometry/geometry.h"
#include "components/comp_base.h"

struct TMsgEntityCreated;

struct TCompAABB : public AABB, public TCompBase {
  bool load(MKeyValue &atts);
  void renderInMenu();
  void render() const;
  void updateFromSiblingsLocalAABBs(CHandle h_entity);
};

struct TCompAbsAABB : public TCompAABB {
  void onCreate(const TMsgEntityCreated&);
};

// Updates AbsAABB from LocalAABB and CompTransform
struct TCompLocalAABB : public TCompAABB {
  void updateAbs();
  void render() const;
  void onCreate(const TMsgEntityCreated&);
};

#endif
