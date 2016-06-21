#ifndef INC_COMPONENT_RENDER_FADE_SCREEN_H_
#define INC_COMPONENT_RENDER_FADE_SCREEN_H_

#include "comp_base.h"



// ------------------------------------
struct TCompFadeScreen : public TCompBase {
  bool enabled;
  bool load(MKeyValue& atts);
  const CRenderTechnique *tech;

  float t_max_fade;
  float t_curr_fade;
  
  void init();

  void renderInMenu();
  void ActiveFade();
  void update(float elapsed);
  void SetMaxTime(float new_time);
  void DeactivateFade();

  void render();

};

#endif
