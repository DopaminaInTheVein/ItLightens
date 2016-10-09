#ifndef INC_FX_GLOW_H_
#define INC_FX_GLOW_H_

#include "app_modules\render\comp_basic_fx.h"
#include <vector>

struct TBlurStep;

// ------------------------------------
struct TRenderGlow : public TCompBasicFX {
public:
  bool enabled;
  void init();
  void renderInMenu();


  std::vector< TBlurStep* > steps;
  VEC4  weights;
  VEC4  distance_factors;     // 1 2 3 4
  float global_distance;
  int   nactive_steps;
  CTexture * apply(CTexture * input, const CRenderTechnique * technique = nullptr);


  void ApplyFX() {
	  //can't be applied directly, need a texture as input
  }

  const char* getName() const {
	  return "blur";
  }

};

#endif
