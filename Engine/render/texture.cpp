#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "texture.h"
#include "DDSTextureLoader.h"
#include "imgui/imgui.h"

#include "app_modules/imgui/module_imgui.h"

#define IMAGE_W_DEBUG 127
#define IMAGE_H_DEBUG 127

template<> IResource::eType getTypeOfResource<CTexture>() { return IResource::TEXTURE; }

template<>
IResource* createObjFromName<CTexture>(const std::string& name) {
	CTexture* texture = new CTexture;
	if (!texture->load(name.c_str())) {
		dbg("Can't load texture %s. Will try placeholder...\n", name.c_str());
		// try to load a placeholder...
		texture = new CTexture;
		if (!texture->load("textures/missing.dds")) {
			fatal("Can't load texture %s\n", name.c_str());
		}
	}
	texture->setName(name.c_str());
	return texture;
}

// ----------------------------------------------
void CTexture::renderUIDebug() {
	ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
	float tex_w = IMAGE_W_DEBUG;
	float tex_h = IMAGE_H_DEBUG;
	static int zoom_w = 127;
	static int zoom_h = 127;
	ImTextureID tex_id = (ImTextureID*)res_view;
	ImGui::Image(tex_id, ImVec2(IMAGE_W_DEBUG, IMAGE_H_DEBUG));

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		float focus_sz = 32.0f;
		float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > tex_w - focus_sz) focus_x = tex_w - focus_sz;
		float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > tex_h - focus_sz) focus_y = tex_h - focus_sz;
		ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
		ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
		ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
		ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
		ImGui::Image(tex_id, ImVec2(zoom_w, zoom_h), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		ImGui::EndTooltip();
	}

	ImGui::DragInt("zoom with", &zoom_w);
	ImGui::DragInt("zoom height", &zoom_h);
	//renderTexture();

	if (ImGui::Button("new texture path")) {
		std::string newTexture = CImGuiModule::getFilePath();
		if (newTexture.size() > 2) { //not empty
			setName(newTexture.c_str());
			loadFullPath(getName().c_str());
		}
	}
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
		, (ID3D11Resource**)&resource
		, &res_view
	);
	if (FAILED(hr))
		return false;

	return true;
}

//-------------------------------------------------
bool CTexture::loadFullPath(const char* filename) {
	std::string full_path = filename;

	// Load the Texture
	wchar_t wname[MAX_PATH];
	mbstowcs(wname, full_path.c_str(), MAX_PATH);

	HRESULT hr;
	hr = DirectX::CreateDDSTextureFromFile(
		Render.device
		, wname
		, (ID3D11Resource**)&resource
		, &res_view
	);
	if (FAILED(hr))
		return false;

	return true;
}

bool CTexture::reload() {
	if (ImGui::Button("reload texture")) {
		if (getName().find(':') != std::string::npos)
			return loadFullPath(getName().c_str());
		else
			return load(getName().c_str());
	}
	return false;
}

// ------------------------------------------------
bool CTexture::create(
	int nxres
	, int nyres
	, DXGI_FORMAT nformat
	, TCreateOptions options)
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