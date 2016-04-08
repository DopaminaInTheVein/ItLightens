#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "texture.h"
#include "DDSTextureLoader.h"

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
void CTexture::deactivate(int slot) {
  ID3D11ShaderResourceView* rs = nullptr;
  Render.ctx->PSSetShaderResources(slot, 1, &rs);
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
    , (ID3D11Resource**) &resource
    , &res_view
    );
  if (FAILED(hr))
    return false;

  return true;
}

// ------------------------------------------------
bool CTexture::create( 
    int nxres
  , int nyres
  , DXGI_FORMAT nformat
  , TCreateOptions options ) 
{
  
  xres = nxres;
  yres = nyres;
  
  D3D11_TEXTURE2D_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  desc.Width = xres;
  desc.Height = yres;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = nformat;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;

  if (options == CREATE_DYNAMIC) {
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  }
  else if (options == CREATE_RENDER_TARGET) {
    desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
  } 
  else {
    assert(options == CREATE_STATIC);
  }

  HRESULT hr = Render.device->CreateTexture2D(&desc, nullptr, &resource);
  if (FAILED(hr))
    return false;
  setDXName(resource, getName().c_str());

  // -----------------------------------------
  // Create a resource view so we can use the data in a shader
  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
  ZeroMemory(&srv_desc, sizeof(srv_desc));
  srv_desc.Format = nformat;
  srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srv_desc.Texture2D.MipLevels = desc.MipLevels;
  hr = Render.device->CreateShaderResourceView(resource, &srv_desc, &res_view);
  if (FAILED(hr))
    return false;
  setDXName(res_view, getName().c_str());

  Resources.registerNew(this);

  return true;
}