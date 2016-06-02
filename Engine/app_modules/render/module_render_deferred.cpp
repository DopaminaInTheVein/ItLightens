#include "mcv_platform.h"
#include "module_render_deferred.h"
#include "camera/camera.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_camera.h"
#include "components/comp_light_dir.h"
#include "components/comp_light_dir_shadows.h"
#include "components/comp_light_point.h"
<<<<<<< HEAD
=======
#include "components/comp_render_glow.h"
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
#include "render/render.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "render/draw_utils.h"
#include "render/render_instanced.h"

// ------------------------------------------------------
bool CRenderDeferredModule::start() {
  xres = CApp::get().getXRes();
  yres = CApp::get().getYRes();

  rt_albedos = new CRenderToTexture;
  rt_normals = new CRenderToTexture;
  rt_depths = new CRenderToTexture;
  rt_acc_light = new CRenderToTexture;

  if (!rt_albedos->createRT("rt_albedo", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_normals->createRT("rt_normals", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_depths->createRT("rt_depths", xres, yres, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_acc_light->createRT("rt_acc_light", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;

  acc_light_points = Resources.get("pbr_point_lights.tech")->as<CRenderTechnique>();
  assert(acc_light_points && acc_light_points->isValid());

  acc_light_directionals = Resources.get("deferred_lights_dir.tech")->as<CRenderTechnique>();
  assert(acc_light_directionals && acc_light_directionals->isValid());

  acc_light_directionals_shadows = Resources.get("deferred_lights_dir_shadows.tech")->as<CRenderTechnique>();
  assert(acc_light_directionals_shadows && acc_light_directionals_shadows->isValid());

  unit_sphere = Resources.get("meshes/unit_sphere.mesh")->as<CMesh>();
  //unit_sphere = Resources.get("unitQuadXY.mesh")->as<CMesh>();
  assert(unit_sphere && unit_sphere->isValid());
  unit_cube = Resources.get("meshes/unit_frustum.mesh")->as<CMesh>();
  assert(unit_cube && unit_cube->isValid());

  Resources.get("textures/noise.dds")->as<CTexture>()->activate(TEXTURE_SLOT_NOISE );
  shader_ctes_globals.world_time = 0.f;

  auto particle_mesh = Resources.get("textured_quad_xy_centered.mesh")->as<CMesh>();
  if (!render_particles_instanced.create(256, particle_mesh))
    return false;

  return true;
}

// ------------------------------------------------------
void CRenderDeferredModule::update( float dt ) {
  shader_ctes_globals.world_time += dt;

  render_particles_instanced.update(dt);
}
// ------------------------------------------------------
void CRenderDeferredModule::stop() {
}

// ------------------------------------------------------
void CRenderDeferredModule::renderGBuffer() {

  PROFILE_FUNCTION("GBuffer");
  CTraceScoped scope("GBuffer");
<<<<<<< HEAD
  static CCamera camera;
=======
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427

  h_camera = tags_manager.getFirstHavingTag(getID("the_camera"));
  if (!h_camera.isValid())
    return;

<<<<<<< HEAD
  CEntity* e = h_camera;
  TCompCamera* comp_cam = e->get<TCompCamera>();
  camera = *comp_cam;
  camera.setAspectRatio((float)xres / (float)yres);
    
  // Copy the render aspect ratio back to the comp_camera
  // of the entity, so the culling uses the real view_proj
  comp_cam->setAspectRatio(camera.getAspectRatio());

=======
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
  // To set a default and known Render State
  Render.ctx->RSSetState(nullptr);
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);

  // -------------------------
  // Activar mis multiples render targets
  ID3D11RenderTargetView* rts[3] = {
    rt_albedos->getRenderTargetView()
  ,	rt_normals->getRenderTargetView()
  ,	rt_depths->getRenderTargetView()
  };
  // Y el ZBuffer del backbuffer principal
  Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
  rt_albedos->activateViewport();

  // Clear de los render targets y el ZBuffer
  rt_albedos->clear(VEC4(1, 0, 0, 1));
  rt_normals->clear(VEC4(0, 1, 0, 1));
  rt_depths->clear(VEC4(1, 1, 1, 1));
  Render.clearMainZBuffer();

<<<<<<< HEAD
  // Activo la camara en la pipeline de render
  activateCamera(&camera);

  // Activa la ctes del object
  activateWorldMatrix(MAT44::Identity);

  // Mandar a pintar los 'solidos'
  RenderManager.renderAll(h_camera, CRenderManager::SOLID_OBJS);
=======
  activateRenderCamera3D();

  // Mandar a pintar los 'solidos'
  RenderManager.renderAll(h_camera, CRenderTechnique::SOLID_OBJS);
}

// ----------------------------------------------
void CRenderDeferredModule::activateRenderCamera3D() {
  CEntity* e = h_camera;
  TCompCamera* comp_cam = e->get<TCompCamera>();
  comp_cam->setAspectRatio((float)xres / (float)yres);

  // Copy the render aspect ratio back to the comp_camera
  // of the entity, so the culling uses the real view_proj
  comp_cam->setAspectRatio(comp_cam->getAspectRatio());

  // Activo la camara en la pipeline de render
  activateCamera(comp_cam);

  // Activa la ctes del object
  activateWorldMatrix(MAT44::Identity);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
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
void CRenderDeferredModule::addDirectionalLightsShadows() {
  PROFILE_FUNCTION("addDirectionalLightsShadows");
  CTraceScoped scope("addDirectionalLightsShadows");

  // Activar la tech acc_light_directionals_shadows.tech
  acc_light_directionals_shadows->activate();

  // Activar la mesh solo UNA vez
  const CMesh* mesh = unit_cube;
  mesh->activate();

  // Activar la mesh unit_sphere
  getHandleManager<TCompLightDirShadows>()->each([mesh](TCompLightDirShadows* c) {
    // Subir todo lo que necesite la luz para pintarse en el acc light buffer
    // la world para la mesh y las constantes en el pixel shader
    c->activate();
    // Pintar la mesh que hemos activado hace un momento
    mesh->render();
  });

  CTexture::deactivate( TEXTURE_SLOT_SHADOWMAP );
}

void CRenderDeferredModule::addAmbientPass() {
<<<<<<< HEAD
=======
  
  activateZ(ZCFG_ALL_DISABLED);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427

  auto tech = Resources.get("pbr_ambient.tech")->as<CRenderTechnique>();
  tech->activate();

  auto mesh = Resources.get("unitQuadXY.mesh")->as<CMesh>();
  mesh->activateAndRender();

}

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
  rt_depths->activate(TEXTURE_SLOT_DEPTHS);
  rt_normals->activate(TEXTURE_SLOT_NORMALS);
  
  addAmbientPass();
  
  //rt_acc_light->clear(VEC4(0, 0, 0, 1));

  activateBlend(BLENDCFG_ADDITIVE);
  activateZ(ZCFG_LIGHTS_CONFIG);
  activateRS(RSCFG_INVERT_CULLING);
  addPointLights();
  /*
  activateZ(ZCFG_LIGHTS_CONFIG);
  //activateRS(RSCFG_INVERT_CULLING);
  addDirectionalLights();
  addDirectionalLightsShadows();
  */

  activateRS(RSCFG_DEFAULT);
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);

  CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
  CTexture::deactivate(TEXTURE_SLOT_NORMALS);
  CTexture::deactivate(TEXTURE_SLOT_DEPTHS);
}

// ----------------------------------------------
void CRenderDeferredModule::generateShadowMaps() {
  PROFILE_FUNCTION("generateShadowMaps");
  CTraceScoped scope("generateShadowMaps");
  
  // Llamar al metodo generateShadowMap para todas los components de tipo dir_shadows
  getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::generateShadowMap);
}

// ----------------------------------------------
void CRenderDeferredModule::render() {
<<<<<<< HEAD

=======
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
  generateShadowMaps();

  shader_ctes_globals.uploadToGPU();

  renderGBuffer();
  renderAccLight();

  // Clear the back buffer
  //float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; // red,green,blue,alpha
  //Render.ctx->ClearRenderTargetView(Render.render_target_view, ClearColor);
  activateZ(ZConfig::ZCFG_TEST_BUT_NO_WRITE);
  activateBlend(BLENDCFG_COMBINATIVE);
  render_particles_instanced.render();
  activateZ(ZConfig::ZCFG_DEFAULT);

<<<<<<< HEAD
  Render.activateBackBuffer();

  activateZ(ZCFG_ALL_DISABLED);
  drawFullScreen(rt_acc_light);
=======
  applyPostFX();

  renderUI();

  // Leave the 3D Camera active
  activateRenderCamera3D();

}

void CRenderDeferredModule::applyPostFX() {
  CTraceScoped scope("applyPostFX");
  CEntity* e_camera = h_camera;
  if( !e_camera )
    return;
  
  CTexture* next_step = rt_acc_light;

  TCompRenderGlow* glow = e_camera->get< TCompRenderGlow >();
  if (glow) 
    next_step = glow->apply(next_step);

  // ------------------------
  Render.activateBackBuffer();

  activateZ(ZCFG_ALL_DISABLED);
  drawFullScreen(next_step);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
  activateZ(ZCFG_DEFAULT);

  // Mandar a pintar los 'transparentes'
  rt_depths->activate(TEXTURE_SLOT_DEPTHS);
<<<<<<< HEAD
  RenderManager.renderAll(h_camera, CRenderManager::TRANSPARENT_OBJS);
  CTexture::deactivate(TEXTURE_SLOT_DEPTHS);
}
=======
  RenderManager.renderAll(h_camera, CRenderTechnique::TRANSPARENT_OBJS);
  CTexture::deactivate(TEXTURE_SLOT_DEPTHS);
}


void CRenderDeferredModule::renderUI() {
  CTraceScoped scope("renderUI");
  activateZ(ZCFG_ALL_DISABLED);
  activateBlend(BLENDCFG_DEFAULT);

  CHandle h_ui_camera = tags_manager.getFirstHavingTag(getID("ui_camera"));
  if (!h_ui_camera.isValid())
    return;
  CEntity* e_ui_camera = h_ui_camera;

  TCompCamera* ortho = e_ui_camera->get<TCompCamera>();
  //ortho->setOrtho(xres, yres, ortho->getZNear(), ortho->getZFar());
  ortho->setOrtho(10, 10, ortho->getZNear(), ortho->getZFar());
  activateCamera(ortho);

  /*

  CCamera ortho;
  ortho.setProjection(deg2rad(60), -1.f, 1.f);
  ortho.setOrtho(xres, yres);
  activateCamera(&ortho);

  // 
  auto tech = Resources.get("ui.tech")->as<CRenderTechnique>();
  tech->activate();

  auto texture = Resources.get("textures/logo.dds")->as<CTexture>();
  texture->activate(TEXTURE_SLOT_DIFFUSE);

  MAT44 world;
  world =
    MAT44::CreateTranslation(1, 1, 0)
    * MAT44::CreateScale(0.5, 0.5, 1)
    * MAT44::CreateScale(512, 400, 1)
    * MAT44::CreateTranslation(0, 400, 0)
    ;  
  
  world =
    MAT44::CreateTranslation(1, 1, 0)
    * MAT44::CreateScale(0.5, 0.5, 1)
    * MAT44::CreateScale(2, 2, 1)
    * MAT44::CreateTranslation(1, 1, 0)
    ;
  activateWorldMatrix(world);

  auto mesh = Resources.get("unitQuadXY.mesh")->as<CMesh>();
  mesh->activateAndRender();
  */
  RenderManager.renderAll(h_ui_camera, CRenderTechnique::UI_OBJS);

}
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
