#include "mcv_platform.h"
#include "resource.h"
#include "resources_manager.h"
#include "render/mesh.h"
#include "imgui/imgui.h"

CResourcesManager Resources;

enum eType {
  UNDEFINED
  , MESH
  , SKIN_MESH
  , TEXTURE
  , SHADER
  , NUM_RESOURCES_TYPE
};
const char* IResource::getTypeName( IResource::eType atype ) {
  switch (atype) {
  case UNDEFINED: return "undefined";
  case MESH: return "Mesh";
  case SKIN_MESH: return "SkinMesh";
  case TEXTURE: return "Texture";
  case SHADER: return "Shader";
  }
  return "invalid";
}

const IResource* CResourcesManager::get(const char* name) {

  // Do we have this object?
  auto it = all.find(name);

  // Yes, we have...
  if (it != all.end())
    return it->second;

  // Try to load...
  std::string ext(name);
  auto p = ext.find_last_of(".");
  ext = ext.substr(p);

  IResource* new_obj = nullptr;

  if (ext == ".mesh") {
    new_obj = createObjFromName<CMesh>(name);
  }
  else {
    fatal("Invalid resource type %s at %s\n", ext.c_str(), name);
  }

  all[name] = new_obj;
  return new_obj;
}

// -------------------------------------
void CResourcesManager::renderUIDebug() {

  ImGui::Begin("Resources");

  // Para cada tipo
  for (int i = IResource::UNDEFINED+1; i < IResource::NUM_RESOURCES_TYPE; ++i) {
    auto res_type = (IResource::eType)(i);
    const char* res_type_name = IResource::getTypeName(res_type);
    if (ImGui::TreeNode(res_type_name)) {

      // Para todos los resources
      for (auto it : all) {
        auto r = it.second;
        if (r->getType() == res_type) {
          if (ImGui::TreeNode(it.first.c_str())) {
            r->renderUIDebug();
            ImGui::TreePop();
          }
        }
      }
      ImGui::TreePop();
    }
  }
  ImGui::End();
}

