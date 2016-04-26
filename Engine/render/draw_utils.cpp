#include "mcv_platform.h"
#include "draw_utils.h"
#include "render/mesh.h"
#include "resources/resources_manager.h"
#include "camera/camera.h"

CShaderCte< TCteCamera > shader_ctes_camera;
CShaderCte< TCteObject > shader_ctes_object;
CShaderCte< TCteBones >  shader_ctes_bones;
CShaderCte< TCteLight >  shader_ctes_lights;

enum samplers {
	eCLAMP = 0,
	eWRAP,
	eSAMPLER_COUNT
};

ID3D11SamplerState* samplers[eSAMPLER_COUNT];



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

bool createSamplers() 
{
	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;

	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = Render.device->CreateSamplerState(
		&sampDesc, &samplers[eWRAP]);
	if (FAILED(hr)) { return false; }

	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Render.device->CreateSamplerState(
		&sampDesc, &samplers[eCLAMP]);
	if (FAILED(hr)) { return false; }

	return true;
}

void destroySamplers()
{
	for (int i = 0; i < eSAMPLER_COUNT; ++i) {
		if(samplers[i])(samplers[i]->Release());
	}
}

void activateSamplers()
{
	Render.ctx->PSSetSamplers(0, eSAMPLER_COUNT, samplers);
}

// -----------------------------------------------
/*void drawLine(const VEC3& src, const VEC3& dst, const VEC4& color) {
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
}*/

bool drawUtilsCreate() {

  if (!shader_ctes_camera.create("ctes_camera"))
    return false;
  if (!shader_ctes_object.create("ctes_object"))
    return false;
  if (!shader_ctes_bones.create("ctes_bones"))
    return false;
  if (!shader_ctes_lights.create("ctes_light"))
    return false;

  if (!createSamplers())
	  return false;

  
  activateDefaultStates();
  activateSamplers();
  return true;
}

void activateDefaultStates() {
  shader_ctes_camera.activate(CTE_SHADER_CAMERA_SLOT);
  shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
  shader_ctes_bones.activate(CTE_SHADER_BONES_SLOT);
  shader_ctes_lights.activate(CTE_SHADER_LIGHT);
  activateZ(ZCFG_DEFAULT);
  activateBlend(BLENDCFG_DEFAULT);
}

void drawUtilsDestroy() {
  shader_ctes_lights.destroy();
  shader_ctes_bones.destroy();
  shader_ctes_camera.destroy();
  shader_ctes_object.destroy();
}

// Activo la camara en la pipeline de render
void activateCamera(const CCamera* camera) {
  shader_ctes_camera.ViewProjection = camera->getViewProjection();
  shader_ctes_camera.CameraWorldPos = VEC4(camera->getPosition());
  shader_ctes_camera.uploadToGPU();
}

// -----------------------------------------------
void activateWorldMatrix(const MAT44& mat) {
  shader_ctes_object.World = mat;
  shader_ctes_object.obj_color = VEC4(1, 1, 1, 1);
  shader_ctes_object.uploadToGPU();
}

// -----------------------------------------------
void drawFullScreen(const CTexture* texture) {
  
  texture->activate(TEXTURE_SLOT_DIFFUSE);

  auto tech = Resources.get("solid_textured.tech")->as<CRenderTechnique>();
  tech->activate();

  activateWorldMatrix(MAT44::Identity);

  TCteCamera prev_cam = shader_ctes_camera;
  shader_ctes_camera.ViewProjection = MAT44::Identity;
  shader_ctes_camera.uploadToGPU();

  auto mesh = Resources.get("unitQuadXY.mesh")->as<CMesh>();
  mesh->activateAndRender();

  TCteCamera* real_ctes_camera = &shader_ctes_camera;
  *real_ctes_camera = prev_cam;
  shader_ctes_camera.uploadToGPU();

}

