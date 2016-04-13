#include "mcv_platform.h"
#include "module_render_deferred.h"
#include "camera/camera.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_camera.h"
#include "render/render.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "render/draw_utils.h"

extern const CRenderTechnique* tech_solid_colored;
extern const CRenderTechnique* tech_textured_colored;

bool CRenderDeferredModule::start() {

  rt = new CRenderToTexture;
  if (!rt->createRT("mainRT", 512, 512, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
    return false;

  return true;
}

void CRenderDeferredModule::stop() {

}

void CRenderDeferredModule::render() {

  {
    PROFILE_FUNCTION("initFrame");
    CTraceScoped scope("initFrame");
    static CCamera camera;

    CHandle h_camera = tags_manager.getFirstHavingTag(getID("camera_main"));
    if (!h_camera.isValid()) return;
    CEntity* e = h_camera;
    TCompCamera* comp_cam = e->get<TCompCamera>();
    camera = *comp_cam;
	  

    // To set a default and known Render State
    Render.ctx->RSSetState(nullptr);
    activateZ(ZCFG_DEFAULT);

    int xres = CApp::get().getXRes();
    int yres = CApp::get().getYRes();

    // -----------
    rt->activateRT();
    rt->clear(VEC4(1, 0, 0, 1));
    
      camera.setAspectRatio((float)xres / (float)yres);

      shader_ctes_camera.activate(CTE_SHADER_CAMERA_SLOT);
	  VEC4 vec = comp_cam->getPosition();
	  shader_ctes_camera.lightvec = float4(1.0f, 1.0f, 1.0f, 0.0f);
	  shader_ctes_camera.lightcol = float4(0.8f, 0.8f, 0.8f, 1.0f);
	  shader_ctes_camera.ambientcol = float4(0.1f, 0.1f, 0.1f, 1.0f);
	  shader_ctes_camera.CamPosition = vec;
      shader_ctes_camera.ViewProjection = camera.getViewProjection();
      shader_ctes_camera.uploadToGPU();

      tech_solid_colored->activate();

      shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
      shader_ctes_object.World = MAT44::Identity;
      shader_ctes_object.uploadToGPU();
      auto axis = Resources.get("axis.mesh")->as<CMesh>();

      axis->activateAndRender();
      Resources.get("grid.mesh")->as<CMesh>()->activateAndRender();
	  
  }

  Render.activateBackBuffer();
  Render.clearMainZBuffer();
  static CCamera camera;
  CHandle h_camera = tags_manager.getFirstHavingTag(getID("camera_main"));
  if (!h_camera.isValid()) return;
  CEntity* e = h_camera;
  TCompCamera* comp_cam = e->get<TCompCamera>();
  camera = *comp_cam;

  shader_ctes_camera.activate(CTE_SHADER_CAMERA_SLOT);
  VEC4 vec = comp_cam->getPosition();
  shader_ctes_camera.lightvec = float4(1.0f, 1.0f, 1.0f, 0.0f);
  shader_ctes_camera.lightcol = float4(0.8f, 0.8f, 0.8f, 1.0f);
  shader_ctes_camera.ambientcol = float4(0.1f, 0.1f, 0.1f, 1.0f);
  shader_ctes_camera.CamPosition = vec;
  shader_ctes_camera.ViewProjection = camera.getViewProjection();
  shader_ctes_camera.uploadToGPU();

  // Clear the back buffer 
  float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; // red,green,blue,alpha
  Render.ctx->ClearRenderTargetView(Render.render_target_view, ClearColor);

  RenderManager.renderAll();


}

