#ifndef INC_FX_HATCHING_H_
#define INC_FX_HATCHING_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderHatching : public TCompBasicFX {

  bool enabled;
  void init();
  void renderInMenu();

  void ApplyFX();

  const char* getName() const {
	  return "hatching";
  }

};

#endif
