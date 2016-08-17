#include "mcv_platform.h"
#include "comp_fading_globe.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "imgui/imgui_internal.h"
#include "resources/resources_manager.h"
#include "render/render.h"
#include "render/DDSTextureLoader.h"

#include "render/shader_cte.h"
#include "constants/ctes_object.h"
#include "constants/ctes_camera.h"
#include "constants/ctes_globals.h"

#include <math.h>

extern CShaderCte< TCteObject > shader_ctes_object;
extern CShaderCte< TCteCamera > shader_ctes_camera;

bool TCompFadingGlobe::load(MKeyValue& atts)
{
	text = atts.getString("text", "defaultText");
	//ttl = atts.getFloat("ttl", 0.1f);
	ttl = timeForLetter * text.length() + 2.0f;
	std::string textColorStr = atts.getString("textColor", "#FFFFFFFF");
	std::string backgroudColorStr = atts.getString("backgroundColor", "#000000FF");
	distance = atts.getFloat("dist", 1.0f);
	char_x = atts.getFloat("posx", 1.0f);
	char_y = atts.getFloat("posy", 1.0f);
	char_z = atts.getFloat("posz", 1.0f);

	float4 char_position = float4(char_x, char_y, char_z, 0.0);
	//float4 world_pos = XMVector4Transform(char_position, shader_ctes_object.World);
	float4 view_pos = XMVector4Transform(char_position, shader_ctes_camera.ViewProjection);

	textColor = obtainColorFromString(textColorStr);
	backgroudColor = obtainColorFromString(backgroudColorStr);
	numchars = 0;

	lines = 1;
	std::string endline = "\n";
	size_t pos = text.find(endline, 0);
	while (pos != text.npos)
	{
		lines++;
		pos = text.find(endline, pos + 1);
	}
	if (lines < minlines) {
		lines = minlines;
	}
	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();
	flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
	marginForImage = lines * percentLineHeight;
	globe_width = 1.0f;
	globe_height = 1.0f;

	return true;
}

void TCompFadingGlobe::update(float dt) {
	static float accumTime = 0.0f;

	accumTime += getDeltaTime();
	while (accumTime > timeForLetter) {
		++numchars;
		accumTime -= timeForLetter;
	}

	if (ttl >= 0.0f) {
		ttl -= getDeltaTime();
	}
	else {
		//textureIcon->destroy();
		CHandle h = CHandle(this).getOwner();
		h.destroy();
	}
}

void TCompFadingGlobe::render() const {
#ifndef NDEBUG
	PROFILE_FUNCTION("TCompFadingMessage render");
	// ttl message is viewed
	std::string textToShow = text.substr(0, numchars);

	bool b = false;

	ImGui::Begin("Game GUI", &b, ImVec2(resolution_x, resolution_y), 0.0f, flags);
	ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));

	Rect rect = GUI::createRect(char_x, char_y, char_x + globe_width, char_y + globe_height);
	GUI::drawRect(rect, backgroudColor);
	GUI::drawText(char_x + percentLineHeight + percentLineHeight + marginForImage, char_y + percentLineHeight, GImGui->Font, sizeFont, textColor, textToShow.c_str());
	ImGui::End();
#endif
}