#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "texture.h"
#include "DDSTextureLoader.h"
#include "imgui/imgui.h"

template<> IResource::eType getTypeOfResource<CTexture>() { return IResource::TEXTURE; }

template<>
IResource* createObjFromName<CTexture>(const std::string& name) {
  CTexture* texture = new CTexture;
  if (!texture->load(name.c_str())) {
    dbg("Can't load texture %s. Will try placeholder...\n", name.c_str());
    // try to load a placeholder...
    if (!texture->load("missing.dds")) {
      fatal("Can't load texture %s\n", name.c_str());
    }
  }
  texture->setName(name.c_str());
  return texture;
}

// ----------------------------------------------
void CTexture::renderUIDebug() {
  ImGui::Image((ImTextureID*)res_view, ImVec2(512, 512));
}

// ----------------------------------------------
void CTexture::activate(int slot) const {
  Render.ctx->PSSetShaderResources(slot, 1, &res_view);
}

// ----------------------------------------------
bool CTexture::load(const char* filename) {

  std::string full_path =
    getDataPath() + std::string(filename);

  // Load the Texture
  wchar_t wname[MAX_PATH];
  mbstowcs(wname, full_path.c_str(), MAX_PATH);

  HRESULT hr;
  hr = DirectX::CreateDDSTextureFromFile(
    Render.device
    , wname
    , &resource
    , &res_view
    );
  if (FAILED(hr))
    return false;

  return true;
}
