#include "mcv_platform.h"
#include "module_render_deferred.h"
#include "camera/camera.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_camera.h"
#include "components/comp_light_dir.h"
#include "components/comp_light_point.h"
#include "render/render.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "render/draw_utils.h"

// ------------------------------------------------------
bool CRenderDeferredModule::start() {
  xres = CApp::get().getXRes();
  yres = CApp::get().getYRes();

  rt_albedos = new CRenderToTexture;
  rt_normals = new CRenderToTexture;
  rt_wpos = new CRenderToTexture;
  rt_acc_light = new CRenderToTexture;

  if (!rt_albedos->createRT("rt_albedo", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_normals->createRT("rt_normals", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_wpos->createRT("rt_wpos", xres, yres, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_acc_light->createRT("rt_acc_light", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;

  acc_light_points = Resources.get("deferred_lights_point.tech")->as<CRenderTechnique>();
  assert(acc_light_points && acc_light_points->isValid());

  acc_light_directionals = Resources.get("deferred_lights_dir.tech")->as<CRenderTechnique>();
  assert(acc_light_directionals && acc_light_directionals->isValid());

  //unit_sphere = Resources.get("meshes/unit_sphere.mesh")->as<CMesh>();
  unit_sphere = Resources.get("unitQuadXY.mesh")->as<CMesh>();
  assert(unit_sphere && unit_sphere->isValid());
  unit_cube = Resources.get("meshes/unit_frustum.mesh")->as<CMesh>();
  assert(unit_cube && unit_cube->isValid());

  return true;
}

// ------------------------------------------------------
void CRenderDeferredModule::stop() {
}

// ------------------------------------------------------
void CRenderDeferredModule::renderGBuffer() {
  PROFILE_FUNCTION("GBuffer");
  CTraceScoped scope("GBuffer");
  static CCamera camera;

  CHandle h_camera = tags_manager.getFirstHavingTag(getID("the_camera"));
  if (h_camera.isValid()) {
    CEntity* e = h_camera;
    TCompCamera* comp_cam = e->get<TCompCamera>();
    camera = *comp_cam;
    camera.setAspectRatio((float)xres / (float)yres);
  }

  // To set a default and known Render State
  Render.ctx->RSSetState(nullptr);
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);

  // -------------------------
  // Activar mis multiples render targets
  ID3D11RenderTargetView* rts[3] = {
    rt_albedos->getRenderTargetView()
    ,	rt_normals->getRenderTargetView()
    ,	rt_wpos->getRenderTargetView()
  };
  // Y el ZBuffer del backbuffer principal
  Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

  // Clear de los render targets y el ZBuffer
  rt_albedos->clear(VEC4(1, 0, 0, 1));
  rt_normals->clear(VEC4(0, 1, 0, 1));
  rt_wpos->clear(VEC4(0, 0, 1, 1));
  Render.clearMainZBuffer();

  // Activo la camara en la pipeline de render
  activateCamera(&camera);

  // Activa la ctes del object
  activateWorldMatrix(MAT44::Identity);

  // Mandar a pintar los 'solidos'
  RenderManager.renderAll(CRenderManager::SOLID_OBJS);
}

// ----------------------------------------------
void CRenderDeferredModule::addPointLights() {
  PROFILE_FUNCTION("addPointLights");
  CTraceScoped scope("addPointLights");

  // Activar la tech deferred_lights_point.tech
  acc_light_points->activate();

  // Activar la mesh solo UNA vez
  const CMesh* mesh = unit_sphere;
  mesh->activate();

  // Activar la mesh unit_sphere
  getHandleManager<TCompLightPoint>()->each([mesh](TCompLightPoint* c) {
    // Subir todo lo que necesite la luz para pintarse en el acc light buffer
    // la world para la mesh y las constantes en el pixel shader
    c->activate();
    // Pintar la mesh que hemos activado hace un momento
    mesh->render();
  });
}

// ----------------------------------------------
void CRenderDeferredModule::addDirectionalLights() {
  PROFILE_FUNCTION("addDirectionalLights");
  CTraceScoped scope("addDirectionalLights");

  // Activar la tech acc_light_directionals.tech
  acc_light_directionals->activate();

  // Activar la mesh solo UNA vez
  const CMesh* mesh = unit_cube;
  mesh->activate();

  // Activar la mesh unit_sphere
  getHandleManager<TCompLightDir>()->each([mesh](TCompLightDir* c) {
    // Subir todo lo que necesite la luz para pintarse en el acc light buffer
    // la world para la mesh y las constantes en el pixel shader
    c->activate();
    // Pintar la mesh que hemos activado hace un momento
    mesh->render();
  });
}

// ----------------------------------------------
void CRenderDeferredModule::renderAccLight() {
  PROFILE_FUNCTION("renderAccLight");
  CTraceScoped scope("renderAccLight");

  // Activar el rt para pintar las luces...
  ID3D11RenderTargetView* rts[3] = {
  rt_acc_light->getRenderTargetView()
  ,	nullptr   // remove the other rt's from the pipeline
  ,	nullptr
  };
  // Y el ZBuffer del backbuffer principal
  Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

  // Activar las texturas del gbuffer en la pipeline para
  // que se puedan acceder desde los siguientes shaders
  rt_albedos->activate(TEXTURE_SLOT_DIFFUSE);
  rt_wpos->activate(TEXTURE_SLOT_WORLD_POS);
  rt_normals->activate(TEXTURE_SLOT_NORMALS);

  rt_acc_light->clear(VEC4(0, 0, 0, 1));

  activateBlend(BLENDCFG_ADDITIVE);
  activateZ(ZCFG_LIGHTS_CONFIG);
  //activateRS(RSCFG_INVERT_CULLING);
  addPointLights();

  activateZ(ZCFG_LIGHTS_CONFIG);
  //activateRS(RSCFG_INVERT_CULLING);
  addDirectionalLights();

  activateRS(RSCFG_DEFAULT);
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);

  CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
  CTexture::deactivate(TEXTURE_SLOT_NORMALS);
  CTexture::deactivate(TEXTURE_SLOT_WORLD_POS);
}

// ----------------------------------------------
void CRenderDeferredModule::render() {
  renderGBuffer();
  renderAccLight();

  Render.activateBackBuffer();

  // Clear the back buffer
  //float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; // red,green,blue,alpha
  //Render.ctx->ClearRenderTargetView(Render.render_target_view, ClearColor);

  activateZ(ZCFG_ALL_DISABLED);
  drawFullScreen(rt_acc_light);
  activateZ(ZCFG_DEFAULT);

  // Mandar a pintar los 'transparentes'
  RenderManager.renderAll(CRenderManager::TRANSPARENT_OBJS);
}