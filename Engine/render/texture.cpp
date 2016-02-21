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
		if (!texture->load("missing.dds")) {
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
		ImGui::Image(tex_id, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		ImGui::EndTooltip();
	}
	//renderTexture();

	if (ImGui::Button("new texture path")) {
		std::string newTexture = CImGuiModule::getFilePath();
		if (newTexture.size() > 2) { //not empty
			setName(newTexture.c_str());
			loadFullPath(name.c_str());
		}
	}
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
		, &resource
		, &res_view
		);
	if (FAILED(hr))
		return false;

	return true;
}

bool CTexture::reload() {
	if (ImGui::Button("reload texture")) {
		if (name.find(':') != std::string::npos)
			return loadFullPath(name.c_str());
		else
			return load(name.c_str());
	}
	return false;
}