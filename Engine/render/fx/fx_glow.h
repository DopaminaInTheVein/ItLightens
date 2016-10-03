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
  void update(float dt);
  void render();
  void renderInMenu();

  void ApplyFX() {
	  //can't be applied directly, need a texture as input
  }

  const char* getName() const {
	  return "blur";
  }

};

#endif
