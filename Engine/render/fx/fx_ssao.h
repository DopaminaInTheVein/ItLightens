#ifndef INC_FX_SSAO_H_
#define INC_FX_SSAO_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderSSAO : public TCompBasicFX {

  void init();
  void renderInMenu();

  void GetOcclusionTextue(CHandle h_camera, CRenderToTexture * rt_ssao, CRenderToTexture * rt_acc_light, CRenderToTexture * rt_shadows_gl);
  void GetInvOcclusionTextue(CHandle h_camera, CRenderToTexture * rt_ssao, CRenderToTexture * rt_acc_light, CRenderToTexture * rt_shadows_gl);

  void GetTexture(CHandle h_camera, CRenderToTexture * rt_ssao, CRenderToTexture * rt_acc_light, CRenderToTexture * rt_shadows_gl);

  const CRenderTechnique *tech_inv;
  const CRenderTechnique *tech_multiple_targets;

  void ApplyFX();

  const char* getName() const {
	  return "dream_border";
  }

};

#endif
