#ifndef INC_COMPONENT_RENDER_GLOW_H_
#define INC_COMPONENT_RENDER_GLOW_H_

#include "comp_base.h"
#include <vector>

struct TBlurStep;

// ------------------------------------
struct TCompRenderGlow : public TCompBase {
  std::vector< TBlurStep* > steps;
  VEC4  weights;
  VEC4  distance_factors;     // 1 2 3 4
  float global_distance;
  int   nactive_steps;
  bool enabled;
  bool load(MKeyValue& atts);
  CTexture * apply(CTexture * input, const CRenderTechnique * technique = nullptr);
  void renderInMenu();

};

#endif
