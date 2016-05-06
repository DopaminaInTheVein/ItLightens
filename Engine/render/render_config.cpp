#include "mcv_platform.h"
#include "render/render.h"

static ID3D11RasterizerState*   rasterizer_states[RSCFG_COUNT];
static ID3D11DepthStencilState* depth_stencil_states[ZCFG_COUNT];
static ID3D11BlendState*        blend_states[BLENDCFG_COUNT];

enum SMPConfig {
  SMP_DEFAULT = 0,
  SMP_BORDER_BLACK = 1, // light dirs
	SMP_CLAMP,
	SMP_PCF_SHADOWS,
  SMP_COUNT,
};
static ID3D11SamplerState*      sampler_states[SMP_COUNT];

// --------------------------------------------------
void createSamplerStates() {
  HRESULT hr;
  D3D11_SAMPLER_DESC desc;

  // Valor por defecto, wrap textures and use mipmaps
  ZeroMemory(&desc, sizeof(desc));
  desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  desc.MaxLOD = D3D11_FLOAT32_MAX;
  hr = Render.device->CreateSamplerState(&desc, &sampler_states[SMP_DEFAULT]);
  assert(!FAILED(hr));
  setDXName(depth_stencil_states[SMP_DEFAULT], "SMP_DEFAULT");

  ZeroMemory(&desc, sizeof(desc));
  desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  desc.MinLOD = 0;
  desc.MaxLOD = D3D11_FLOAT32_MAX;
  hr = Render.device->CreateSamplerState(&desc, &sampler_states[SMP_CLAMP]);
  assert(!FAILED(hr));
  setDXName(depth_stencil_states[SMP_CLAMP], "SMP_CLAMP");

  // Para las luces direccionales, si me salgo del espacio homogeneo
  // usar el color 'negro' para iluminar.
  ZeroMemory(&desc, sizeof(desc));
  desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
  desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
  desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
  desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  desc.MaxLOD = D3D11_FLOAT32_MAX;
  desc.BorderColor[0] = 0.f;
  desc.BorderColor[1] = 0.f;
  desc.BorderColor[2] = 0.f;
  desc.BorderColor[3] = 0.f;
  hr = Render.device->CreateSamplerState(&desc, &sampler_states[SMP_BORDER_BLACK]);
  assert(!FAILED(hr));
  setDXName(depth_stencil_states[SMP_BORDER_BLACK], "SMP_BORDER_BLACK");

  // PCF sampling
  D3D11_SAMPLER_DESC sampler_desc = {
    D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,// D3D11_FILTER Filter;
    D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressU;
    D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressV;
    D3D11_TEXTURE_ADDRESS_BORDER, //D3D11_TEXTURE_ADDRESS_MODE AddressW;
    0,//FLOAT MipLODBias;
    0,//UINT MaxAnisotropy;
    D3D11_COMPARISON_LESS, //D3D11_COMPARISON_FUNC ComparisonFunc;
    0.0, 0.0, 0.0, 0.0,//FLOAT BorderColor[ 4 ];
    0,//FLOAT MinLOD;
    0//FLOAT MaxLOD;   
  };
  hr = ::Render.device->CreateSamplerState( &sampler_desc, &sampler_states[SMP_PCF_SHADOWS]);
  assert(!FAILED(hr));
  setDXName(sampler_states[SMP_PCF_SHADOWS], "PCF_SHADOWS");

}

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

  //mark mask
  //-------------------------------------------------------------------------
  desc.DepthEnable = true;
  desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  desc.DepthFunc = D3D11_COMPARISON_EQUAL;

  desc.StencilEnable = true;
  desc.StencilReadMask = 0xFF;
  desc.StencilWriteMask = 0xFF;

  desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
  desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

  desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  //we don't care about back facing pixels
  desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
  desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

  hr = Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_MASK_NUMBER]);
  assert(!FAILED(hr));
  setDXName(depth_stencil_states[ZCFG_MASK_NUMBER], "ZCFG_MASK_NUMBER");


  //discard
  //-------------------------------------------------------------------------
  desc.DepthEnable = true;
  desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  desc.DepthFunc = D3D11_COMPARISON_LESS;

  desc.StencilEnable = true;
  desc.StencilReadMask = 0xFF;

  desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
  desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

  // The stencil test passes if the passed parameter is equal to value in the buffer.
  desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

  // Again, we don't care about back facing pixels.
  desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
  desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

  hr = Render.device->CreateDepthStencilState(&desc, &depth_stencil_states[ZCFG_OUTLINE]);
  assert(!FAILED(hr));
  setDXName(depth_stencil_states[ZCFG_OUTLINE], "ZCFG_OUTLINE");


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
  desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
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

  // Depth bias options when rendering the shadows
  desc = {
    D3D11_FILL_SOLID, // D3D11_FILL_MODE FillMode;
    D3D11_CULL_BACK,  // D3D11_CULL_MODE CullMode;
    FALSE,            // BOOL FrontCounterClockwise;
    13,               // INT DepthBias;
    0.0f,             // FLOAT DepthBiasClamp;
    2.0f,             // FLOAT SlopeScaledDepthBias;
    TRUE,             // BOOL DepthClipEnable;
    FALSE,            // BOOL ScissorEnable;
    FALSE,            // BOOL MultisampleEnable;
    FALSE,            // BOOL AntialiasedLineEnable;
  };
  hr = Render.device->CreateRasterizerState(&desc, &rasterizer_states[RSCFG_SHADOWS]);
  assert(!FAILED(hr));
  setDXName(rasterizer_states[RSCFG_SHADOWS], "RS_SHADOWS");
}

// ----------------------------------------------
void createRenderStateConfigs() {
  createZStates();
  createBlendStates();
  createRasterizerStates();
  createSamplerStates();
}

void destroyRenderStateConfigs() {
	for (int i = 0; i < RSCFG_COUNT; ++i)
		SAFE_RELEASE(rasterizer_states[i]);

	for (int i = 0; i < ZCFG_COUNT; ++i)
		SAFE_RELEASE(depth_stencil_states[i]);

  for (int i = 0; i < BLENDCFG_COUNT; ++i)
    		SAFE_RELEASE(blend_states[i]);

  for (int i = 0; i < SMP_COUNT; ++i)
    		SAFE_RELEASE(sampler_states[i]);
}

void activateZ(enum ZConfig id, UINT num) {
	Render.ctx->OMSetDepthStencilState(depth_stencil_states[id], num);
}

void activateRS(enum RSConfig id) {
	Render.ctx->RSSetState(rasterizer_states[id]);
}

void activateBlend(enum BlendConfig id) {
  const float blend_factor[4] = { 0,0,0,0 };
  Render.ctx->OMSetBlendState(blend_states[id], blend_factor, 0xffffffff);
}

void activateSamplerStates() {
  Render.ctx->PSSetSamplers(0, SMP_COUNT, sampler_states);
}

// ----------------------------------