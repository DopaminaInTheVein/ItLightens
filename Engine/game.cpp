#include "mcv_platform.h"
#include "windows/app.h"
#include "render/mesh.h"
#include "render/shaders.h"
#include "render/vertex_declarations.h"
#include "render/shader_cte.h"
#include "render/technique.h"
#include "render/texture.h"
#include "resources/resources_manager.h"
#include "camera/camera.h"
#include "app_modules/app_module.h"
#include "app_modules/imgui/module_imgui.h"
#include "input\input.h"


#include "logic/sbb.h"
#include "handle/object_manager.h"
#include "components/comp_transform.h"
#include "components/entity_tags.h"
#include "components/entity.h"

CCamera       camera;
CInput        input;

const CRenderTechnique* tech_solid_colored = nullptr;
const CRenderTechnique* tech_textured_colored = nullptr;
const CTexture*         texture1 = nullptr;

#include "contants/ctes_camera.h"
CShaderCte< TCteCamera > shader_ctes_camera;
#include "contants/ctes_object.h"
CShaderCte< TCteObject > shader_ctes_object;

// --------------------------------------------
#include "app_modules/entities.h"

bool CApp::start() {

   //init input controller
   input.Initialize(CApp::getHInstance(), CApp::getHWnd(), 800, 600);

  // imgui must be the first to update and the last to render
  auto imgui = new CImGuiModule;
  auto entities = new CEntitiesModule;
  
  // Will contain all modules created
  all_modules.push_back(imgui);
  all_modules.push_back(entities);
  
  mod_update.push_back(imgui);
  mod_update.push_back(entities);
  mod_renders.push_back(entities);
  mod_renders.push_back(imgui);
  mod_init_order.push_back(imgui);
  mod_init_order.push_back(entities);
  mod_wnd_proc.push_back(imgui);

  // ----------------------------
  tech_solid_colored = Resources.get("tech_solid_colored.tech")->as<CRenderTechnique>();
  tech_textured_colored = Resources.get("tech_textured_colored.tech")->as<CRenderTechnique>();
  texture1 = Resources.get("textures/wood_d.dds")->as<CTexture>();

  if (!shader_ctes_camera.create("ctes_camera"))
    return false;
  if (!shader_ctes_object.create("ctes_object"))
    return false;

  camera.lookAt(VEC3(10, 4, 2), VEC3(1, 0, 2));

  // Init modules
  for (auto it : mod_init_order) {
    if (!it->start()) {
      dbg("Failed to init module %s\n", it->getName());
      return false;
    }
  }

  return true;
}

// ----------------------------------
void CApp::stop() {

  // Stop input
  input.Shutdown();

  // Stop modules
  for (auto it = mod_init_order.rbegin(); it != mod_init_order.rend(); ++it) 
    (*it)->stop();

  Resources.destroy();
  shader_ctes_camera.destroy();
  shader_ctes_object.destroy();

  // Delete all modules
  for (auto m : all_modules)
    delete m;
  all_modules.clear();
}

// ----------------------------------
void CApp::update(float elapsed) {

	// Update input
	input.Frame();

	if (input.IsUpPressed())
	{
		dbg("ARRIBA!\n");
	}
	if (input.IsDownPressed())
	{
		dbg("ABAJO!\n");
	}
	if (input.IsLeftPressed())
	{
		dbg("IZQUIERDA!\n");
	}
	if (input.IsRightPressed())
	{
		dbg("DERECHA!\n");
	}
	if (input.IsSpacePressed()) {
		dbg("SALTO!\n");
	}
	if (input.IsLeftClickPressed()) {
		dbg("ACCION!\n");
	}
	if (input.IsRightClickPressed()) {
		dbg("POSESION!\n");
	}

  for (auto it : mod_update )
    it->update(elapsed);

  static float ctime = 0.f;
  ctime += elapsed* 0.01f;
 // camera.lookAt(VEC3(sin(ctime), 1.f, cos(ctime))*4, VEC3(0, 0, 0));

}

// ----------------------------------
void CApp::render() {
  {
    CTraceScoped scope("initFrame");

    // To set a default and known Render State
    Render.ctx->RSSetState(nullptr);

    // Clear the back buffer 
    float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; // red,green,blue,alpha
    Render.ctx->ClearRenderTargetView(Render.renderTargetView, ClearColor);
    camera.setAspectRatio((float)xres / (float)yres);

    shader_ctes_camera.activate(CTE_SHADER_CAMERA_SLOT);
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

  //{
  //  CTraceScoped scope("textured obj");
  //  // el shader de pos + uv
  //  shader_ctes_object.World = MAT44::Identity;
  //  shader_ctes_object.uploadToGPU();

  //  texture1->activate(0);
  //  tech_textured_colored->activate();
  //  Resources.get("meshes/Teapot001.mesh")->as<CMesh>()->activateAndRender();
  //}

  for (auto it : mod_renders) {
    CTraceScoped scope( it->getName() );
    it->render();
  }

}



