#ifndef INC_FX_ANTIALIASING_H_
#define INC_FX_ANTIALIASING_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderAntiAliasing : public TCompBasicFX {

  void init();
  void renderInMenu();

  void ApplyFX();

  const char* getName() const {
	  return "anti aliasing";
  }

};

#endif
