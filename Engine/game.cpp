#include "mcv_platform.h"
#include "windows/app.h"
#include "render/mesh.h"
#include "render/shaders.h"
#include "render/vertex_declarations.h"
#include "render/shader_cte.h"
#include "resources/resources_manager.h"
#include "camera/camera.h"
#include "app_modules/app_module.h"
#include "imgui/imgui.h"
#include "logic/aicontroller.h"
#include "logic/aic_patrol.h"
#include "entities/tentity.h"

CVertexShader vs;
CVertexShader vs_uv;
CPixelShader  ps;
CCamera       camera;

//AI controller
aic_patrol aicp;

#include "contants/ctes_camera.h"
CShaderCte< TCteCamera > shader_ctes_camera;
#include "contants/ctes_object.h"
CShaderCte< TCteObject > shader_ctes_object;

std::vector< TEntity > entities;

// --------------------------------------------
#include "app_modules/imgui/imgui.h"

bool CApp::start() {

  // imgui must be the first to update and the last to render
  auto imgui = new CImGuiModule;
  
  // Will contain all modules created
  all_modules.push_back(imgui);
  
  mod_update.push_back(imgui);
  mod_renders.push_back(imgui);
  mod_init_order.push_back(imgui);
  mod_wnd_proc.push_back(imgui);

  // ----------------------------

  if (!vs.create("data/shaders/Tutorial02.fx", "VS", &vdecl_positions_color))
    return false;
  if (!ps.create("data/shaders/Tutorial02.fx", "PS"))
    return false;
  if (!vs_uv.create("data/shaders/Tutorial02.fx", "VS_UV", &vdecl_positions_uv))
    return false;
  if (!shader_ctes_camera.create())
    return false;
  if (!shader_ctes_object.create())
    return false;

  camera.lookAt(VEC3(-10, 10, 20)*0.7f, VEC3(0, 0, 0));

  // Init modules
  for (auto it : mod_init_order) {
    if (!it->start()) {
      dbg("Failed to init module %s\n", it->getName());
      return false;
    }
  }


  entities.resize(3);
  entities[0].transform.setPosition(VEC3(2.5f, 0, 0));
  entities[1].transform.setPosition(VEC3(2.5f, 0, 2.5f));

  //Init AI controller 
  TEntity patrol_entity;
  entities[2] = patrol_entity;  
  aicp.Init(&entities[2]);

  return true;
}

// ----------------------------------
void CApp::stop() {

  // Stop modules
  for (auto it = mod_init_order.rbegin(); it != mod_init_order.rend(); ++it) 
    (*it)->stop();

  Resources.destroy();
  shader_ctes_camera.destroy();
  shader_ctes_object.destroy();
  ps.destroy();
  vs.destroy();

  // Delete all modules
  for (auto m : all_modules)
    delete m;
  all_modules.clear();
}

// ----------------------------------
void CApp::update(float elapsed) {

  for (auto it : mod_update )
    it->update(elapsed);

  aicp.Recalc();

  Resources.renderUIDebug();

  ImGui::Begin("Entities");
 
  if (ImGui::Button("Add")) {
    dbg("Adding new entity\n");
    TEntity e;
    static int id = 0;
    char tmp[32];
    sprintf(tmp, "E%02d", ++id);
    e.name = tmp;
    //e.transform.setAngles(deg2rad(60), deg2rad(60));
    entities.push_back(e);
  }

  if (ImGui::TreeNode("Entities")) {
    ImGui::Text("%ld entites\n", entities.size());
    int idx = 0;
    for (auto& it : entities) {
      ImGui::PushID(&it);
      if (ImGui::TreeNode(it.name.c_str())) {

        if (ImGui::SmallButton("Delete")) {
          dbg("Deleting entity %s\n", it.name.c_str());
        }

        if (idx > 0) {
          VEC3 target = entities[idx - 1].transform.getPosition();
          float delta_yaw = it.transform.getDeltaYawToAimTo(target);
          ImGui::Text("Angle To Prev Entity: %1.2f", rad2deg(delta_yaw));
          if (ImGui::Button("Look To Prev")) {
            float yaw, pitch;
            it.transform.getAngles(&yaw, &pitch);
            it.transform.setAngles(yaw + delta_yaw * 0.2f, pitch);
          }
          ImGui::Text("isInFront: %s", it.transform.isInFront(target) ? "YES" : "NO");
          ImGui::Text("isInLeft: %s", it.transform.isInLeft(target) ? "YES" : "NO");
          ImGui::Text("isConeVision: %s", it.transform.isHalfConeVision(target, deg2rad(30.f)) ? "YES" : "NO" );
        }

        VEC3 pos = it.transform.getPosition();
        if (ImGui::SliderFloat3("Pos", &pos.x, -10.f, 10.f)) {
          it.transform.setPosition(pos);
        }

        float yaw, pitch;
        it.transform.getAngles(&yaw, &pitch);
        yaw = rad2deg(yaw);
        pitch = rad2deg(pitch);
        bool yaw_changed = ImGui::SliderFloat("Yaw", &yaw, -180.f, 180.f);
        bool pitch_changed = ImGui::SliderFloat("Pitch", &pitch, -90.f+0.001f, 90.f - 0.001f);
        if( yaw_changed || pitch_changed )
          it.transform.setAngles(deg2rad(yaw), deg2rad(pitch));

        ImGui::TreePop();
      }
      ImGui::PopID();
      ++idx;
    }
    ImGui::TreePop();
  }
  ImGui::End();


  static float ctime = 0.f;
  ctime += elapsed* 0.01f;
 // camera.lookAt(VEC3(sin(ctime), 1.f, cos(ctime))*4, VEC3(0, 0, 0));
}

// ----------------------------------
void CApp::render() {
  // To set a default and known Render State
  Render.ctx->RSSetState(nullptr);

	// Clear the back buffer 
	float ClearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f }; // red,green,blue,alpha
	Render.ctx->ClearRenderTargetView(Render.renderTargetView, ClearColor);
  camera.setAspectRatio( (float)xres/(float)yres );

  shader_ctes_camera.activate(CTE_SHADER_CAMERA_SLOT);
  shader_ctes_camera.ViewProjection = camera.getViewProjection();
  shader_ctes_camera.uploadToGPU();

  ps.activate();
  vs.activate();

  shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
  shader_ctes_object.World = MAT44::Identity;
  shader_ctes_object.uploadToGPU();
  auto axis = Resources.get("axis.mesh")->as<CMesh>();

  axis->activateAndRender();
  Resources.get("grid.mesh")->as<CMesh>()->activateAndRender();

  for (auto e : entities) {
    shader_ctes_object.World = e.transform.asMatrix();
    shader_ctes_object.uploadToGPU();
    axis->activateAndRender();
  }

  shader_ctes_object.World = MAT44::CreateTranslation(VEC3(-1, 0, 0));
  shader_ctes_object.uploadToGPU();
  axis->activateAndRender();

  // el shader de pos + uv
  shader_ctes_object.World = MAT44::Identity;
  shader_ctes_object.uploadToGPU();
  vs_uv.activate();
  ps.activate();
  //Resources.get("meshes/Teapot001.mesh")->as<CMesh>()->activateAndRender();

  for (auto it : mod_update)
    it->render();

}



