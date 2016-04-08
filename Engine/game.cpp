#include "mcv_platform.h"
#include "windows/app.h"
#include "render/mesh.h"
#include "render/shaders.h"
#include "render/vertex_declarations.h"
#include "render/shader_cte.h"
#include "render/technique.h"
#include "render/texture.h"
#include "render/render_manager.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "camera/camera.h"
#include "app_modules/app_module.h"
#include "app_modules/imgui/module_imgui.h"
#include "app_modules/io/io.h"

#include "app_modules/render/module_render_deferred.h"
#include "components/entity_parser.h"
#include "handle/handle_manager.h"

const CRenderTechnique* tech_solid_colored = nullptr;
const CRenderTechnique* tech_textured_colored = nullptr;

// --------------------------------------------
#include "app_modules/entities.h"

bool CApp::start() {

  // imgui must be the first to update and the last to render
  auto imgui = new CImGuiModule;
  auto entities = new CEntitiesModule;
  io = new CIOModule;     // It's the global io
  auto render_deferred = new CRenderDeferredModule;
  
  // Will contain all modules created
  all_modules.push_back(imgui);
  all_modules.push_back(entities);
  all_modules.push_back(io);
  all_modules.push_back(render_deferred);
  
  mod_update.push_back(imgui);
  mod_update.push_back(entities);
  mod_update.push_back(io);
  mod_renders.push_back(render_deferred);
  mod_renders.push_back(entities);
  mod_renders.push_back(imgui);
  mod_renders.push_back(io);
  mod_init_order.push_back(imgui);
  mod_init_order.push_back(render_deferred);
  mod_init_order.push_back(io);
  mod_init_order.push_back(entities);
  mod_wnd_proc.push_back(io);
  mod_wnd_proc.push_back(imgui);

  // ----------------------------
  tech_solid_colored = Resources.get("solid_colored.tech")->as<CRenderTechnique>();
  tech_textured_colored = Resources.get("textured.tech")->as<CRenderTechnique>();

  if (!shader_ctes_camera.create("ctes_camera"))
    return false;
  if (!shader_ctes_object.create("ctes_object"))
    return false;
  if (!shader_ctes_bones.create("ctes_bones"))
    return false;
  shader_ctes_bones.activate(CTE_SHADER_BONES_SLOT);

  // Init modules
  for (auto it : mod_init_order) {
    if (!it->start()) {
      dbg("Failed to init module %s\n", it->getName());
      return false;
    }
  }

  //h_camera = tags_manager.getFirstHavingTag(getID("the_camera"));

  return true;
}

// ----------------------------------
void CApp::stop() {

  // Stop modules
  for (auto it = mod_init_order.rbegin(); it != mod_init_order.rend(); ++it) 
    (*it)->stop();

  Resources.destroy();
  shader_ctes_bones.destroy();
  shader_ctes_camera.destroy();
  shader_ctes_object.destroy();

  // Delete all modules
  for (auto m : all_modules)
    delete m;
  all_modules.clear();
}

// ----------------------------------
void CApp::update(float elapsed) {
  PROFILE_FUNCTION("CApp::update");

  for (auto it : mod_update) {
    PROFILE_FUNCTION(it->getName());
    it->update(elapsed);
  }

  {
    PROFILE_FUNCTION("Resources.renderUIDebug");
    Resources.renderUIDebug();
  }

  static float ctime = 0.f;
  ctime += elapsed* 0.01f;

  {
    PROFILE_FUNCTION("destroyAllPendingObjects");
    CHandleManager::destroyAllPendingObjects();
  }
}

// ----------------------------------
void CApp::render() {
  PROFILE_FUNCTION("CApp::render");
  
  for (auto it : mod_renders) {
    PROFILE_FUNCTION(it->getName());
    CTraceScoped scope( it->getName() );
    it->render();
  }

}



