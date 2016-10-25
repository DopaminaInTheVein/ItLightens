#ifndef INC_FX_OUTLINE_H_
#define INC_FX_OUTLINE_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderOutline : public TCompBasicFX {

  void init();
  void renderInMenu();

  void ApplyFX();

  const char* getName() const {
	  return "outline";
  }

};

#endif
