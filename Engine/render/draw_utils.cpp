#include "mcv_platform.h"
#include "draw_utils.h"
#include "render/mesh.h"
#include "resources/resources_manager.h"
#include "camera/camera.h"

CShaderCte< TCteCamera > shader_ctes_camera;
CShaderCte< TCteObject > shader_ctes_object;
CShaderCte< TCteBones >  shader_ctes_bones;
CShaderCte< TCteLight >  shader_ctes_lights;
CShaderCte< TCteGlobals > shader_ctes_globals;
CShaderCte< TCteBlur >     shader_ctes_blur;
CShaderCte< TCteHatching > shader_ctes_hatching;
CShaderCte< TCteGui > shader_ctes_gui;
CShaderCte< TCteDream > shader_ctes_dream;

const CTexture* all_black;

// -----------------------------------------------
bool createDepthBuffer(
	int xres
	, int yres
	, DXGI_FORMAT depth_format
	, ID3D11Texture2D** out_depth_resource
	, ID3D11DepthStencilView** out_depth_stencil_view
	, const char* name
	, CTexture** out_ztexture
) {
	assert(depth_format == DXGI_FORMAT_R32_TYPELESS
		|| depth_format == DXGI_FORMAT_R24G8_TYPELESS
		|| depth_format == DXGI_FORMAT_R16_TYPELESS
		|| depth_format == DXGI_FORMAT_D24_UNORM_S8_UINT
		|| depth_format == DXGI_FORMAT_R8_TYPELESS);

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
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	//  if (bind_shader_resource)
	if (depth_format != DXGI_FORMAT_D24_UNORM_S8_UINT)
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

	// SRV = Shader Resource View
	// DSV = Depth Stencil View
	DXGI_FORMAT texturefmt = DXGI_FORMAT_R32_TYPELESS;
	DXGI_FORMAT SRVfmt = DXGI_FORMAT_R32_FLOAT;       // Stencil format
	DXGI_FORMAT DSVfmt = DXGI_FORMAT_D32_FLOAT;       // Depth format

	switch (depth_format) {
	case DXGI_FORMAT_R32_TYPELESS:
		SRVfmt = DXGI_FORMAT_R32_FLOAT;
		DSVfmt = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		SRVfmt = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		DSVfmt = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	case DXGI_FORMAT_R16_TYPELESS:
		SRVfmt = DXGI_FORMAT_R16_UNORM;
		DSVfmt = DXGI_FORMAT_D16_UNORM;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
		SRVfmt = DXGI_FORMAT_R8_UNORM;
		DSVfmt = DXGI_FORMAT_R8_UNORM;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		SRVfmt = desc.Format;
		DSVfmt = desc.Format;
		break;
	}

	ID3D11Texture2D* depth_resource;
	HRESULT hr = Render.device->CreateTexture2D(&desc, NULL, &depth_resource);
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DSVfmt;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = Render.device->CreateDepthStencilView(depth_resource
		, &descDSV
		, out_depth_stencil_view);
	if (FAILED(hr))
		return false;
	setDXName((*out_depth_stencil_view), "ZTextureDSV");

	*out_depth_resource = depth_resource;

	// ------------------------------------------------------
	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) {
		// Setup the description of the shader resource view.
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = SRVfmt;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = desc.MipLevels;

		// Create the shader resource view to access the texture
		ID3D11ShaderResourceView* depth_resource_view = nullptr;
		hr = Render.device->CreateShaderResourceView(depth_resource, &shaderResourceViewDesc, &depth_resource_view);
		if (FAILED(hr))
			return false;

		// as this is used by the RenderTarget with Z (for the shadows maps) and
		// by the ZTexture, we need to add one more ref because it will be freed twice
		depth_resource->AddRef();

		CTexture* ztexture = new CTexture();
		ztexture->setDXObjs(depth_resource, depth_resource_view);
		ztexture->setName(("Z" + std::string(name)).c_str());
		Resources.registerNew(ztexture);
		setDXName(depth_resource, ztexture->getName().c_str());
		setDXName(depth_resource_view, ztexture->getName().c_str());
		assert(out_ztexture);
		*out_ztexture = ztexture;
	}

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

bool drawUtilsCreate() {
	if (!shader_ctes_globals.create("ctes_globals"))
		return false;
	if (!shader_ctes_camera.create("ctes_camera"))
		return false;
	if (!shader_ctes_gui.create("ctes_gui"))
		return false;
	if (!shader_ctes_object.create("ctes_object"))
		return false;
	if (!shader_ctes_bones.create("ctes_bones"))
		return false;
	if (!shader_ctes_lights.create("ctes_light"))
		return false;
	if (!shader_ctes_blur.create("ctes_blur"))
		return false;
	if (!shader_ctes_hatching.create("ctes_hatching"))
		return false;
	if (!shader_ctes_dream.create("ctes_dream"))
		return false;

	all_black = Resources.get("textures/missing_black.dds")->as<CTexture>();

	activateDefaultStates();
	return true;
}

void activateDefaultStates() {
	shader_ctes_camera.activate(CTE_SHADER_CAMERA_SLOT);
	shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
	shader_ctes_bones.activate(CTE_SHADER_BONES_SLOT);
	shader_ctes_lights.activate(CTE_SHADER_LIGHT);
	shader_ctes_globals.activate(CTE_SHADER_GLOBALS_SLOT);
	shader_ctes_blur.activate(CTE_SHADER_BLUR_SLOT);
	shader_ctes_hatching.activate(CTE_SHADER_HATCHING_SLOT);
	shader_ctes_gui.activate(CTE_SHADER_GUI_SLOT);
	shader_ctes_dream.activate(CTE_SHADER_DREAM_SLOT);
	activateZ(ZCFG_DEFAULT);
	activateBlend(BLENDCFG_DEFAULT);
	activateSamplerStates();
}

void drawUtilsDestroy() {
	shader_ctes_globals.destroy();
	shader_ctes_lights.destroy();
	shader_ctes_bones.destroy();
	shader_ctes_camera.destroy();
	shader_ctes_object.destroy();
	shader_ctes_hatching.destroy();
	shader_ctes_blur.destroy();
	shader_ctes_gui.destroy();
	shader_ctes_dream.destroy();
}

// Activo la camara en la pipeline de render
void activateCamera(const CCamera* camera) {
	shader_ctes_camera.ViewProjection = camera->getViewProjection();
	shader_ctes_camera.CameraWorldPos = VEC4(camera->getPosition());
	shader_ctes_camera.CameraFront = VEC4(camera->getFront());
	shader_ctes_camera.CameraUp = VEC4(camera->getUp());
	shader_ctes_camera.CameraLeft = VEC4(camera->getLeft());
	shader_ctes_camera.CameraZFar = camera->getZFar();
	shader_ctes_camera.CameraZNear = camera->getZNear();
	shader_ctes_camera.CameraTanHalfFov = tan(camera->getFov() * 0.5f);
	shader_ctes_camera.CameraAspectRatio = camera->getAspectRatio();

	shader_ctes_camera.uploadToGPU();
}

// -----------------------------------------------
void activateWorldMatrix(const MAT44& mat) {
	PROFILE_FUNCTION("World Matrix activate");
	shader_ctes_object.World = mat;
	shader_ctes_object.obj_color = VEC4(1, 1, 1, 1);
	shader_ctes_object.uploadToGPU();
}

void drawWiredAABB(const AABB& aabb, const MAT44& world, VEC4 color) {
	// Accede a una mesh que esta centrada en el origen y
	// tiene 0.5 de half size
	auto mesh = Resources.get("wired_unit_cube.mesh")->as<CMesh>();
	MAT44 unit_cube_to_aabb = MAT44::CreateScale(VEC3(aabb.Extents) * 2.f)
		* MAT44::CreateTranslation(aabb.Center)
		* world;
	shader_ctes_object.World = unit_cube_to_aabb;
	shader_ctes_object.obj_color = color;
	shader_ctes_object.uploadToGPU();
	mesh->activateAndRender();
}

// -----------------------------------------------
void drawFullScreen(const CTexture* texture, const CRenderTechnique* tech) {
	texture->activate(TEXTURE_SLOT_DIFFUSE);
	if (!tech)
		tech = Resources.get("solid_textured.tech")->as<CRenderTechnique>();

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