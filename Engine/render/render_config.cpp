#include "mcv_platform.h"
#include "render/render.h"

static ID3D11RasterizerState*   rasterizer_states[RSCFG_COUNT];
static ID3D11DepthStencilState* depth_stencil_states[ZCFG_COUNT];
static ID3D11BlendState*        blend_states[BLENDCFG_COUNT];

// --------------------------------------------------
void createZStates() {
	HRESULT hr;
	// Create depth stencila desc
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Write to the ZBuffer -> YES
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = false;
	hr = Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_DEFAULT]);
	assert(!FAILED(hr));
	setDXName(depth_stencil_states[ZCFG_DEFAULT], "Z_DEFAULT");

	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = false;
	hr = Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_ALL_DISABLED]);
	assert(!FAILED(hr));
	setDXName(depth_stencil_states[ZCFG_ALL_DISABLED], "Z_ALL_DISABLED");

	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = false;
	hr = Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_TEST_BUT_NO_WRITE]);
	assert(!FAILED(hr));
	setDXName(depth_stencil_states[ZCFG_TEST_BUT_NO_WRITE], "Z_TEST_BUT_NO_WRITE");

  // Configuracion a usar cuando pintamos las point lights
  // Quiero iluminar aquellos pixeles que fallen el ztest
  // y NO quiero escribir en el zbuffer
  ZeroMemory(&desc, sizeof(desc));
  desc.DepthEnable = true;
  desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  desc.DepthFunc = D3D11_COMPARISON_ALWAYS;// D3D11_COMPARISON_GREATER;
  desc.StencilEnable = false;
  hr = Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_LIGHTS_CONFIG]);
  assert(!FAILED(hr));
  setDXName(depth_stencil_states[ZCFG_LIGHTS_CONFIG], "Z_LIGHTS_CONFIG");

  // Configuracion a usar cuando pintamos las dir lights
  // Quiero iluminar aquellos pixeles que fallen el ztest
  // y NO quiero escribir en el zbuffer
  ZeroMemory(&desc, sizeof(desc));
  desc.DepthEnable = true;
  desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  desc.DepthFunc = D3D11_COMPARISON_GREATER;
  desc.StencilEnable = false;
  hr = Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_LIGHTS_DIR_CONFIG]);
  assert(!FAILED(hr));
  setDXName(depth_stencil_states[ZCFG_LIGHTS_DIR_CONFIG], "Z_LIGHTS_DIR_CONFIG");

}

// --------------------------------------------------
void createBlendStates() {
	HRESULT hr;
	// Create rasterize desc
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//hr = Render.device->CreateBlendState(&desc, &blend_states[BLENDCFG_DEFAULT]);
	//setDXName(blend_states[BLENDCFG_DEFAULT], "BLEND_DEFAULT");
	blend_states[BLENDCFG_DEFAULT] = nullptr;

	ZeroMemory(&desc, sizeof(desc));
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = Render.device->CreateBlendState(&desc, &blend_states[BLENDCFG_ADDITIVE]);
	assert(!FAILED(hr));
	setDXName(blend_states[BLENDCFG_ADDITIVE], "BLEND_ADDITIVE");

	ZeroMemory(&desc, sizeof(desc));
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	// Usar el alpha salida del pixel shader (src_alpha)
	// como cantidad que se tiene que modular el color salida del pixel
	// shader antes de sumarlo a la aportacion del color destino
	//   RGB_Final = RGB_Source * SrcBlend + RGB_Dest * DestBlend
	// Donde SrcBlend = RGB_Source.a
	//       DestBlend = 1-RGB_Source.a
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	hr = Render.device->CreateBlendState(&desc, &blend_states[BLENDCFG_COMBINATIVE]);
	assert(!FAILED(hr));
	setDXName(blend_states[BLENDCFG_COMBINATIVE], "BLEND_COMBINATIVE");
}

// ----------------------------------------------
void createRasterizerStates() {
	HRESULT hr;
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_FRONT;
	desc.DepthClipEnable = TRUE;
	hr = Render.device->CreateRasterizerState(&desc, &rasterizer_states[RSCFG_INVERT_CULLING]);
	assert(!FAILED(hr));
	setDXName(rasterizer_states[RSCFG_INVERT_CULLING], "INVERT_CULLING");
}

// ----------------------------------------------
void createRenderStateConfigs() {
	createZStates();
	createBlendStates();
	createRasterizerStates();
}

void destroyRenderStateConfigs() {
	for (int i = 0; i < RSCFG_COUNT; ++i)
		SAFE_RELEASE(rasterizer_states[i]);

	for (int i = 0; i < ZCFG_COUNT; ++i)
		SAFE_RELEASE(depth_stencil_states[i]);

	for (int i = 0; i < BLENDCFG_COUNT; ++i)
		SAFE_RELEASE(blend_states[i]);
}

void activateZ(enum ZConfig id) {
	Render.ctx->OMSetDepthStencilState(depth_stencil_states[id], 0);
}

void activateRS(enum RSConfig id) {
	Render.ctx->RSSetState(rasterizer_states[id]);
}

void activateBlend(enum BlendConfig id) {
	const float blend_factor[4] = { 0,0,0,0 };
	Render.ctx->OMSetBlendState(blend_states[id], blend_factor, 0xffffffff);
}

// ----------------------------------