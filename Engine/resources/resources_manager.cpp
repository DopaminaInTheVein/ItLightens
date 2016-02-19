#include "mcv_platform.h"
#include "resource.h"
#include "resources_manager.h"
#include "render/mesh.h"
#include "render/technique.h"
#include "render/texture.h"
#include "imgui/imgui.h"

CResourcesManager Resources;


const char* IResource::getTypeName( IResource::eType atype ) {
  switch (atype) {
  case IResource::UNDEFINED: return "undefined";
  case IResource::MESH:      return "Meshes";
  case IResource::TECHNIQUE: return "Techniques";
  case IResource::VERTEX_SHADER: return "VertexShaders";
  case IResource::PIXEL_SHADER: return "PixelShaders";
  case IResource::TEXTURE: return "Textures";
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
  else if (ext == ".tech") {
    new_obj = createObjFromName<CRenderTechnique>(name);
  }
  else if (ext == ".vs") {
    new_obj = createObjFromName<CVertexShader>(name);
  }
  else if (ext == ".ps") {
    new_obj = createObjFromName<CPixelShader>(name);
  }
  else if (ext == ".dds") {
    new_obj = createObjFromName<CTexture>(name);
  }
  else {
    fatal("Invalid resource type %s at %s\n", ext.c_str(), name);
  }

  all[name] = new_obj;
  return new_obj;
}

// -------------------------------------
void CResourcesManager::renderUIDebug() {

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
}

