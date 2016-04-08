#include "mcv_platform.h"
#include "draw_utils.h"
#include "render/mesh.h"
#include "resources/resources_manager.h"

CShaderCte< TCteCamera > shader_ctes_camera;
CShaderCte< TCteObject > shader_ctes_object;
CShaderCte< TCteBones >  shader_ctes_bones;

// -----------------------------------------------
bool createDepthBuffer(
    int xres
  , int yres
  , DXGI_FORMAT depth_format
  , ID3D11Texture2D** out_depth_resource
  , ID3D11DepthStencilView** out_depth_stencil_view
  ) {
  // Create depth stencil texture
  D3D11_TEXTURE2D_DESC desc;
  ZeroMemory(&desc, sizeof(desc));
  desc.Width = xres;
  desc.Height = yres;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = depth_format;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    //| D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  ID3D11Texture2D* depth_resource;
  HRESULT hr = Render.device->CreateTexture2D(&desc, NULL, &depth_resource);
  if (FAILED(hr))
    return false;

  // Create the depth stencil view
  D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
  ZeroMemory(&descDSV, sizeof(descDSV));
  descDSV.Format = desc.Format;
  descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  descDSV.Texture2D.MipSlice = 0;
  hr = Render.device->CreateDepthStencilView(depth_resource
    , &descDSV
    , out_depth_stencil_view);
  if (FAILED(hr))
    return false;

  *out_depth_resource = depth_resource;
  return true;
}

// -----------------------------------------------
void drawLine(const VEC3& src, const VEC3& dst, const VEC4& color) {
  MAT44 world = MAT44::CreateLookAt(src, dst, VEC3(0, 1, 0)).Invert();
  float distance = VEC3::Distance(src, dst);
  world = MAT44::CreateScale(1, 1, -distance) * world;
  shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
  shader_ctes_object.World = world;
  shader_ctes_object.obj_color = color;
  shader_ctes_object.uploadToGPU();
  auto line = Resources.get("line.mesh")->as<CMesh>();
  line->activateAndRender();
  shader_ctes_object.obj_color = VEC4(1, 1, 1, 1);
}

