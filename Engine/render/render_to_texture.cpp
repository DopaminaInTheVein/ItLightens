#include "mcv_platform.h"
#include "render_to_texture.h"
#include "draw_utils.h"

CRenderToTexture::CRenderToTexture() 
: color_format( DXGI_FORMAT_UNKNOWN )
, render_target_view( nullptr )
, depth_stencil_view( nullptr )
, ztexture( nullptr )
{  }
  
CRenderToTexture::~CRenderToTexture() {

}

void CRenderToTexture::destroy() {
  SAFE_RELEASE(render_target_view);
  SAFE_RELEASE(depth_stencil_view);
  SAFE_RELEASE(depth_resource);
  CTexture::destroy();
}

bool CRenderToTexture::createRT(
    const char* new_name
  , int new_xres
  , int new_yres
  , DXGI_FORMAT new_color_format
  , DXGI_FORMAT new_depth_format
  ) {

  xres = new_xres;
  yres = new_yres;
  setName(new_name);

  // Create color buffer
  color_format = new_color_format;
  if (color_format != DXGI_FORMAT_UNKNOWN) {
    if (!create(new_xres, new_yres, color_format, CREATE_RENDER_TARGET))
      return false;
    HRESULT hr = Render.device->CreateRenderTargetView(resource, nullptr, &render_target_view);
    if (FAILED(hr))
      return false;
    setDXName(render_target_view, getName().c_str());
  }

  // Create ZBuffer 
  depth_format = new_depth_format;
  if (depth_format != DXGI_FORMAT_UNKNOWN) {
    if (!createDepthBuffer(xres, yres, new_depth_format, &depth_resource, &depth_stencil_view, new_name, &ztexture))
      return false;
  }

  return true;
}

void CRenderToTexture::activateRT() {
  Render.ctx->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
  activateViewport();
}

void CRenderToTexture::activateViewport() {
  D3D11_VIEWPORT vp;
  vp.Width = (float)xres;
  vp.Height = (float)yres;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  vp.MinDepth = 0.f;
  vp.MaxDepth = 1.f;
  Render.ctx->RSSetViewports(1, &vp);
}

void CRenderToTexture::clear(VEC4 clear_color) {
  assert(render_target_view);
  Render.ctx->ClearRenderTargetView( render_target_view, &clear_color.x);
}

void CRenderToTexture::clearZ() {
  assert(depth_stencil_view);
  Render.ctx->ClearDepthStencilView( depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
