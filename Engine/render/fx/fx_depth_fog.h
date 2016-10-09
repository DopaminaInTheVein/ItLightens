#ifndef INC_FX_FOG_H_
#define INC_FX_FOG_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderDepthFog : public TCompBasicFX {

  bool enabled;
  void init();
  void renderInMenu();

  void ApplyFX();

  const char* getName() const {
	  return "fog depth";
  }

};

#endif
