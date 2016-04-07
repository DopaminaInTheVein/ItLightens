#ifndef INC_COMPONENT_SKELETON_H_
#define INC_COMPONENT_SKELETON_H_

#include "components/comp_base.h"

class CalModel;
class CSkeleton;

struct TCompSkeleton : public TCompBase {
  const CSkeleton* resource_skeleton;
  CalModel*  model;
  bool load(MKeyValue& atts);
  void update(float dt);
  void render() const;
  void renderInMenu();
  void uploadBonesToCteShader() const;
};

#endif

