#include "mcv_platform.h"
#include "render/render.h"

static ID3D11RasterizerState*   rasterizer_states[RSCFG_COUNT];
static ID3D11DepthStencilState* depth_stencil_states[ZCFG_COUNT];

void createRenderStateConfigs() {
  
  // Create depth stencila desc
  {
    D3D11_DEPTH_STENCIL_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.DepthEnable = true;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Write to the ZBuffer -> YES
    desc.DepthFunc = D3D11_COMPARISON_LESS;
    desc.StencilEnable = false;
    Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_DEFAULT]);
    setDXName(depth_stencil_states[ZCFG_DEFAULT], "Z_DEFAULT");

    ZeroMemory(&desc, sizeof(desc));
    desc.DepthEnable = false;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    desc.StencilEnable = false;
    Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_ALL_DISABLED]);
    setDXName(depth_stencil_states[ZCFG_ALL_DISABLED], "Z_ALL_DISABLED");
  }

  {
    // Create rasterize desc
    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_NONE;
    desc.ScissorEnable = true;
    desc.DepthClipEnable = true;
    Render.device->CreateRasterizerState(&desc, &rasterizer_states[ RSCFG_DEFAULT ]);
    setDXName(rasterizer_states[RSCFG_DEFAULT], "RS_DEFAULT");
  }
}

void destroyRenderStateConfigs() {
  
  for (int i = 0; i < RSCFG_COUNT; ++i)
    SAFE_RELEASE(rasterizer_states[i]);

  for (int i = 0; i < ZCFG_COUNT; ++i)
    SAFE_RELEASE(depth_stencil_states[i]);
}

void activateZ(enum ZConfig id) {
  Render.ctx->OMSetDepthStencilState(depth_stencil_states[id], 0);
}

void activateRS( enum RSConfig id) {
  Render.ctx->RSSetState(rasterizer_states[id]);
}

// ----------------------------------
