#include "mcv_platform.h"
#include "comp_light_dir_shadows.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"

bool TCompLightDirShadows::load(MKeyValue& atts) {
  TCompLightDir::load(atts);

  int res = atts.getInt("resolution", 256);
  rt_shadows = new CRenderToTexture();
  // I don't need a color buffer, just the ZBuffer
  bool is_ok = rt_shadows->createRT("ShadowMap", Render.getXRes(), Render.getYRes(), DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_TYPELESS);
  setObjName(rt_shadows, "rt_shadows");
  assert(is_ok);

  enabled = atts.getBool("enabled", true);

  return is_ok;
}

void TCompLightDirShadows::update(float dt) {
  PROFILE_FUNCTION("shadows: update");
  CHandle owner = CHandle(this).getOwner();
  updateFromEntityTransform(owner);
}

void TCompLightDirShadows::activate() {
	PROFILE_FUNCTION("shadows: activate");
  CHandle owner = CHandle(this).getOwner();
  activateWorldMatrix(getViewProjection().Invert());
  rt_shadows->getZTexture()->activate(TEXTURE_SLOT_SHADOWMAP);
  uploadShaderCtes(owner);
}

void TCompLightDirShadows::generateShadowMap() {
  if (!enabled)
    return;
  assert(rt_shadows);

  // Vamos a empezar a pintar en el shadow map
  rt_shadows->activateRT();
  rt_shadows->clearZ();
  activateRS(RSCFG_SHADOWS);

  // Desde MI punto de vista, el pt de vista de la luz direccional
  // que genera sombras
  activateCamera(this);

  // activar la tech de shadow map generation
  Resources.get("shadow_gen.tech")->as<CRenderTechnique>()->activate();

  // Pintar los shadow casters
  RenderManager.renderShadowCasters(CHandle(this).getOwner());

  // activar la tech de shadow map generation
  Resources.get("shadow_gen_skin.tech")->as<CRenderTechnique>()->activate();

  // Pintar los shadow casters
  RenderManager.renderShadowCastersSkin(CHandle(this).getOwner());

  activateRS(RSCFG_DEFAULT);
}

void TCompLightDirShadows::destroy() {
	//if(rt_shadows)rt_shadows->destroy();
}

void TCompLightDirShadows::setNewFov(float fov_in_rads) {
  float fov_in_degs = rad2deg(fov_in_rads);
  setProjection(fov_in_rads, getZNear(), getZFar());
}