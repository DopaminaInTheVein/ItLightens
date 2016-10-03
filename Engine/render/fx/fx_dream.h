#ifndef INC_FX_DREAM_H_
#define INC_FX_DREAM_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderDream : public TCompBasicFX {

  bool enabled;
  void init();
  void renderInMenu();

  void ApplyFX();

  const char* getName() const {
	  return "dream effect";
  }

};

#endif
