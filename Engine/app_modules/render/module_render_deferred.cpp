#include "mcv_platform.h"
#include "module_render_deferred.h"
#include "camera/camera.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_camera.h"
#include "components/comp_light_dir.h"
#include "components/comp_light_dir_shadows.h"
#include "components/comp_light_point.h"
#include "components/comp_light_fadable.h"
#include "render/render.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "render/draw_utils.h"

#include "render/fx/GuardShots.h"

//POLARIZE
#include "components/comp_polarized.h"
#include "components\comp_transform.h"
#include "components\comp_render_static_mesh.h"
#include "render\static_mesh.h"

// ------------------------------------------------------
bool CRenderDeferredModule::start() {
  //xres = CApp::get().getXRes();
  //yres = CApp::get().getYRes();

  xres = Render.getXRes();
  yres = Render.getYRes();

  rt_albedos = new CRenderToTexture;
  rt_normals = new CRenderToTexture;
  rt_wpos = new CRenderToTexture;
  rt_acc_light = new CRenderToTexture;
  rt_selfIlum = new CRenderToTexture;
  rt_selfIlum_blurred = new CRenderToTexture;
  rt_depthTexture = new CRenderToTexture;
  rt_final = new CRenderToTexture;

  //aux
  rt_data = new CRenderToTexture;
  rt_black = new CRenderToTexture;

  if (!rt_albedos->createRT("rt_albedo", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_data->createRT("rt_data", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_black->createRT("rt_black", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_normals->createRT("rt_normals", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_wpos->createRT("rt_wpos", xres, yres, DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_selfIlum->createRT("rt_selfIlum", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_acc_light->createRT("rt_acc_light", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_selfIlum_blurred->createRT("rt_selfIlum_blurred", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_final->createRT("rt_final", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;
  if (!rt_depthTexture->createRT("rt_depthTexture", xres, yres, DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_UNKNOWN))
    return false;
  //
  acc_light_points = Resources.get("deferred_lights_point.tech")->as<CRenderTechnique>();
  assert(acc_light_points && acc_light_points->isValid());

  acc_light_directionals = Resources.get("deferred_lights_dir.tech")->as<CRenderTechnique>();
  assert(acc_light_directionals && acc_light_directionals->isValid());

  blur_tech = Resources.get("blur.tech")->as<CRenderTechnique>();

  acc_light_directionals_shadows = Resources.get("deferred_lights_dir_shadows.tech")->as<CRenderTechnique>();
  assert(acc_light_directionals_shadows && acc_light_directionals_shadows->isValid());

  //unit_sphere = Resources.get("meshes/unit_sphere.mesh")->as<CMesh>();
  unit_sphere = Resources.get("unitQuadXY.mesh")->as<CMesh>();
  assert(unit_sphere && unit_sphere->isValid());
  unit_cube = Resources.get("meshes/unit_frustum.mesh")->as<CMesh>();
  assert(unit_cube && unit_cube->isValid());

  Resources.get("textures/noise.dds")->as<CTexture>()->activate(TEXTURE_SLOT_NOISE );
  shader_ctes_globals.world_time = 0.f;
  shader_ctes_globals.xres = xres;
  shader_ctes_globals.yres = yres;
  shader_ctes_globals.strenght_polarize = 1.0f / 5.0f;

  return true;
}

// ------------------------------------------------------
void CRenderDeferredModule::stop() {
}

// ------------------------------------------------------
void CRenderDeferredModule::update(float dt) {
  shader_ctes_globals.world_time += dt;
}

// ------------------------------------------------------
void CRenderDeferredModule::renderGBuffer() {
  PROFILE_FUNCTION("GBuffer");
  CTraceScoped scope("GBuffer");
  static CCamera camera;

  h_camera = tags_manager.getFirstHavingTag(getID("camera_main"));
  if (h_camera.isValid()) {
    CEntity* e = h_camera;
    TCompCamera* comp_cam = e->get<TCompCamera>();
    camera = *comp_cam;
    camera.setAspectRatio((float)xres / (float)yres);
    comp_cam->setAspectRatio(camera.getAspectRatio());
  }

  // To set a default and known Render State
  Render.ctx->RSSetState(nullptr);
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);

  // Activo la camara en la pipeline de render
  activateCamera(&camera);

  // -------------------------
  // Activar mis multiples render targets
  ID3D11RenderTargetView* rts[4] = {
    rt_albedos->getRenderTargetView()
    ,	rt_normals->getRenderTargetView()
    ,	rt_wpos->getRenderTargetView()
    ,   rt_selfIlum->getRenderTargetView()
  };
  // Y el ZBuffer del backbuffer principal
  Render.ctx->OMSetRenderTargets(4, rts, Render.depth_stencil_view);
  rt_albedos->activateViewport();
  // Clear de los render targets y el ZBuffer
  rt_albedos->clear(VEC4(1, 0, 0, 1));
  rt_normals->clear(VEC4(0, 1, 0, 1));
  rt_selfIlum->clear(VEC4(0, 0, 0, 1));
  rt_wpos->clear(VEC4(0, 0, 1, 1));
  rt_depthTexture->clear(VEC4(0, 0, 0, 1));
  rt_final->clear(VEC4(0, 0, 0, 1));
  Render.clearMainZBuffer();

  rt_acc_light->clear(VEC4(0, 0, 0, 1));
  rt_selfIlum_blurred->clear(VEC4(0, 0, 0, 1));

  // Activa la ctes del object
  activateWorldMatrix(MAT44::Identity);

  // Mandar a pintar los 'solidos'
  RenderManager.renderAll(h_camera, CRenderManager::SOLID_OBJS);

  activateZ(ZCFG_DEFAULT);
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

void CRenderDeferredModule::FinalRender() {
  PROFILE_FUNCTION("final_texture");
  CTraceScoped scope("final_texture");

  ID3D11RenderTargetView* rts[3] = {
    rt_final->getRenderTargetView()
    ,	nullptr   // remove the other rt's from the pipeline
    ,	nullptr
  };
  // Y el ZBuffer del backbuffer principal
  Render.ctx->OMSetRenderTargets(3, rts, nullptr);

  rt_albedos->activate(TEXTURE_SLOT_DIFFUSE);
  rt_acc_light->activate(TEXTURE_SLOT_ENVIRONMENT);
  rt_selfIlum->activate(TEXTURE_SLOT_SELFILUM);
  rt_wpos->activate(TEXTURE_SLOT_WORLD_POS);
  rt_normals->activate(TEXTURE_SLOT_NORMALS);

  activateZ(ZCFG_ALL_DISABLED);

  auto tech = Resources.get("deferred_add_ambient.tech")->as<CRenderTechnique>();
  drawFullScreen(rt_albedos, tech);

  activateZ(ZCFG_DEFAULT);

  CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
  CTexture::deactivate(TEXTURE_SLOT_NORMALS);
  CTexture::deactivate(TEXTURE_SLOT_SELFILUM);
  CTexture::deactivate(TEXTURE_SLOT_ENVIRONMENT);
  CTexture::deactivate(TEXTURE_SLOT_WORLD_POS);
}

void CRenderDeferredModule::blurEffectLights() {
  PROFILE_FUNCTION("blurEffectLights");
  CTraceScoped scope("blurEffectLights");

  rt_selfIlum_blurred->clear(VEC4(0, 0, 0, 0));

  ID3D11RenderTargetView* rts[3] = {
    rt_selfIlum_blurred->getRenderTargetView()
    ,	nullptr   // remove the other rt's from the pipeline
    ,	nullptr
  };
  // Y el ZBuffer del backbuffer principal
  Render.ctx->OMSetRenderTargets(3, rts, nullptr);

  activateZ(ZCFG_ALL_DISABLED);

  //auto tech = Resources.get("deferred_add_ambient.tech")->as<CRenderTechnique>();
  drawFullScreen(rt_selfIlum, blur_tech);

  activateZ(ZCFG_DEFAULT);
  CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
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

  addDirectionalLightsShadows();

  activateRS(RSCFG_DEFAULT);
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);

  CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
  CTexture::deactivate(TEXTURE_SLOT_NORMALS);
  CTexture::deactivate(TEXTURE_SLOT_WORLD_POS);
}

// ----------------------------------------------
void CRenderDeferredModule::generateShadowMaps() {
  PROFILE_FUNCTION("generateShadowMaps");
  CTraceScoped scope("generateShadowMaps");
  
  // Llamar al metodo generateShadowMap para todas los components de tipo dir_shadows
  getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::generateShadowMap);
}

void CRenderDeferredModule::DepthTexture() {
  PROFILE_FUNCTION("final_texture");
  CTraceScoped scope("final_texture");

  ID3D11RenderTargetView* rts[3] = {
    rt_depthTexture->getRenderTargetView()
    ,	nullptr   // remove the other rt's from the pipeline
    ,	nullptr
  };
  // Y el ZBuffer del backbuffer principal

  Render.ctx->OMSetRenderTargets(3, rts, nullptr);

  rt_final->activate(TEXTURE_SLOT_DIFFUSE);
  rt_acc_light->activate(TEXTURE_SLOT_ENVIRONMENT);
  rt_selfIlum_blurred->activate(TEXTURE_SLOT_SELFILUM);
  rt_wpos->activate(TEXTURE_SLOT_WORLD_POS);
  rt_normals->activate(TEXTURE_SLOT_NORMALS);

  activateZ(ZCFG_ALL_DISABLED);

  auto tech = Resources.get("depthTexture.tech")->as<CRenderTechnique>();
  drawFullScreen(rt_final, tech);

  activateZ(ZCFG_DEFAULT);

  CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
  CTexture::deactivate(TEXTURE_SLOT_NORMALS);
  CTexture::deactivate(TEXTURE_SLOT_SELFILUM);
  CTexture::deactivate(TEXTURE_SLOT_ENVIRONMENT);
  CTexture::deactivate(TEXTURE_SLOT_WORLD_POS);
}

void CRenderDeferredModule::RenderPolarizedPP(int pol, const VEC4& color) {
  shader_ctes_globals.global_color = color;
  shader_ctes_globals.uploadToGPU();

  //create mask
  {
    PROFILE_FUNCTION("referred: mask");
    CTraceScoped scope("mask");

    //activateZ(ZCFG_DEFAULT);
    activateZ(ZCFG_MASK_NUMBER, pol);

    ID3D11RenderTargetView* rts[3] = {
      rt_black->getRenderTargetView()
      ,	nullptr   // remove the other rt's from the pipeline
      ,	nullptr
    };
    Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

    auto tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
    tech->activate();

    getHandleManager<TCompPolarized>()->each([pol](TCompPolarized* c) {
      if (c->force.polarity == pol) {	//render polarity designed only
        CEntity *e = CHandle(c).getOwner();
        TCompRenderStaticMesh *rsm = e->get<TCompRenderStaticMesh>();
        TCompTransform *c_tmx = e->get<TCompTransform>();

        activateWorldMatrix(c_tmx->asMatrix());
        // Si algun dia tenemos mas de un slot, revisar esto!
        assert(rsm->static_mesh->slots.size() == 1);
        rsm->static_mesh->slots[0].material->activateTextures();
        rsm->static_mesh->slots[0].mesh->activateAndRender();

        rsm->static_mesh->slots[0].material->deactivateTextures();
      }
    });
  }

  //edge detection
  {
    PROFILE_FUNCTION("referred: edge detection");
    CTraceScoped scope("edge detection final");

    // Activar el rt para pintar las luces...

    ID3D11RenderTargetView* rts[3] = {
      rt_data->getRenderTargetView()
      ,	nullptr   // remove the other rt's from the pipeline
      ,	nullptr
    };
    // Y el ZBuffer del backbuffer principal

    Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

    rt_wpos->activate(TEXTURE_SLOT_WORLD_POS);
    rt_normals->activate(TEXTURE_SLOT_NORMALS);

    rt_depthTexture->activate(45);

    activateZ(ZCFG_OUTLINE, pol);
    //activateZ(ZCFG_ALL_DISABLED);

    auto tech = Resources.get("edgeDetection.tech")->as<CRenderTechnique>();

    drawFullScreen(rt_black, tech);
    CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
    //rt_black->clear(VEC4(0, 0, 0, 1)); //we dont care about that texture, clean black texture
  }
}

void CRenderDeferredModule::GlowEdges() {
  {
    PROFILE_FUNCTION("referred: GlowEdges");
    CTraceScoped scope("glow edges");

    ID3D11RenderTargetView* rts1[3] = {
      rt_selfIlum->getRenderTargetView()
      ,	nullptr   // remove the other rt's from the pipeline
      ,	nullptr
    };
    // Y el ZBuffer del backbuffer principal
    Render.ctx->OMSetRenderTargets(3, rts1, Render.depth_stencil_view);

    auto tech = Resources.get("solid_PP.tech")->as<CRenderTechnique>();
    drawFullScreen(rt_data, tech);

    blurEffectLights();

    tech = Resources.get("solid_PP.tech")->as<CRenderTechnique>();

    activateBlend(BLENDCFG_ADDITIVE);
    Render.activateBackBuffer();				//render on screen
    drawFullScreen(rt_selfIlum_blurred, tech);

    activateBlend(BLENDCFG_DEFAULT);
  }
}

// ----------------------------------------------
void CRenderDeferredModule::render() {
  //Render.clearMainZBuffer();
  rt_black->clear(VEC4(0, 0, 0, 1));
  rt_data->clear(VEC4(0, 0, 0, 0));

  generateShadowMaps();

  shader_ctes_globals.uploadToGPU();
  renderGBuffer();

  renderAccLight();

  blurEffectLights();

  FinalRender();
  DepthTexture();

  // Fx
  {
    PROFILE_FUNCTION("referred: laser");
    CTraceScoped scope("laser");

    ID3D11RenderTargetView* rts[3] = {
      rt_final->getRenderTargetView()
      ,	nullptr   // remove the other rt's from the pipeline
      ,	nullptr
    };
    Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
    //Render.activateBackBuffer();
    activateZ(ZCFG_DEFAULT);
    ShootManager::renderAll();
  }

  Render.activateBackBuffer();

  activateZ(ZCFG_ALL_DISABLED);
  drawFullScreen(rt_final);
  activateZ(ZCFG_DEFAULT);
  
  if (GameController->GetFxPolarize()) {
    RenderPolarizedPP(MINUS, VEC4(1.0f, 0.3f, 0.3f, 1.0f));
    RenderPolarizedPP(PLUS, VEC4(0.3f, 0.3f, 1.0f, 1.0f));
  }
  activateZ(ZCFG_ALL_DISABLED);

  if (GameController->GetFxGlow()) GlowEdges();

  

  CTexture::deactivate(45);
  CTexture::deactivate(TEXTURE_SLOT_NORMALS);
  CTexture::deactivate(TEXTURE_SLOT_WORLD_POS);
  
  Render.activateBackBuffer();
  activateZ(ZCFG_DEFAULT);

  // Mandar a pintar los 'transparentes'
  RenderManager.renderAll(h_camera, CRenderManager::TRANSPARENT_OBJS);
}