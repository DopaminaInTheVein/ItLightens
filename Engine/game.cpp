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
#include "imgui/imgui.h"

CCamera       camera;

const CRenderTechnique* tech_solid_colored = nullptr;
const CRenderTechnique* tech_textured_colored = nullptr;
const CTexture*         texture1 = nullptr;

#include "contants/ctes_camera.h"
CShaderCte< TCteCamera > shader_ctes_camera;
#include "contants/ctes_object.h"
CShaderCte< TCteObject > shader_ctes_object;

// --------------------------------
struct TEntity {
  CTransform   transform;
  std::string  name;
  //IResource*   mesh;
  //const CMesh* mesh;
};

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
  tech_solid_colored = Resources.get("tech_solid_colored.tech")->as<CRenderTechnique>();
  tech_textured_colored = Resources.get("tech_textured_colored.tech")->as<CRenderTechnique>();
  texture1 = Resources.get("textures/wood_d.dds")->as<CTexture>();

  if (!shader_ctes_camera.create("ctes_camera"))
    return false;
  if (!shader_ctes_object.create("ctes_object"))
    return false;

  camera.lookAt(VEC3(-10, 10, 20)*0.7f, VEC3(0, 0, 0));

  // Init modules
  for (auto it : mod_init_order) {
    if (!it->start()) {
      dbg("Failed to init module %s\n", it->getName());
      return false;
    }
  }

  entities.resize(2);
  entities[0].transform.setPosition(VEC3(2.5f, 0, 0));
  entities[1].transform.setPosition(VEC3(2.5f, 0, 2.5f));

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

  // Delete all modules
  for (auto m : all_modules)
    delete m;
  all_modules.clear();
}

// ----------------------------------
void CApp::update(float elapsed) {

  for (auto it : mod_update )
    it->update(elapsed);

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

  {
    CTraceScoped scope("entities");
    auto axis = Resources.get("axis.mesh")->as<CMesh>();
    for (auto e : entities) {
      shader_ctes_object.World = e.transform.asMatrix();
      shader_ctes_object.uploadToGPU();
      axis->activateAndRender();
    }
  }

  {
    CTraceScoped scope("textured obj");
    // el shader de pos + uv
    shader_ctes_object.World = MAT44::Identity;
    shader_ctes_object.uploadToGPU();

    texture1->activate(0);
    tech_textured_colored->activate();
    Resources.get("meshes/Teapot001.mesh")->as<CMesh>()->activateAndRender();
  }

  for (auto it : mod_update) {
    CTraceScoped scope( it->getName() );
    it->render();
  }

}



