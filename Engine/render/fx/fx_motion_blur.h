#ifndef INC_FX_MOTION_BLURG_H_
#define INC_FX_MOTION_BLURG_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderMotionBlur : public TCompBasicFX {

  bool enabled;
  void init();
  void renderInMenu();

  void ApplyFX();

  const char* getName() const {
	  return "motion blur";
  }

};

#endif
