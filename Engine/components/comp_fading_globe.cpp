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
#include "comp_camera_main.h"

#include <math.h>

extern CShaderCte< TCteObject > shader_ctes_object;
extern CShaderCte< TCteCamera > shader_ctes_camera;

bool TCompFadingGlobe::load(MKeyValue& atts)
{
	text = atts.getString("text", "defaultText");
	ttl = 1.0f;
	std::string textColorStr = atts.getString("textColor", "#FFFFFFFF");
	std::string backgroudColorStr = atts.getString("backgroundColor", "#000000FF");
	distance = atts.getFloat("dist", 1.0f);
	char_x = atts.getFloat("posx", 1.0f);
	char_y = atts.getFloat("posy", 1.0f);
	char_z = atts.getFloat("posz", 1.0f);

	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();

	dbg("world pos: %f, %f, %f\n", char_x, char_y, char_z);

	float aspect_ratio = shader_ctes_camera.CameraAspectRatio;
	float zfar = shader_ctes_camera.CameraZFar;
	float znear = shader_ctes_camera.CameraZNear;

	float ys = 1.0f / shader_ctes_camera.CameraTanHalfFov;
	float xs = ys / aspect_ratio;

	float proj_x = char_x * xs;
	float proj_y = char_y * ys;
	float proj_z = char_z * (zfar / (zfar - znear)) - (zfar*znear / (zfar - znear));

	float4 proj_coords = float4(proj_x, proj_y, proj_z, char_z);
	proj_coords.Normalize();

	screen_x = (proj_coords.x + 1.0f) / 2.0f;
	screen_y = (1.f - proj_coords.y) / 2.0f;

	dbg("screen pos: %f, %f\n", screen_x, screen_y);

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

	flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
	marginForImage = lines * percentLineHeight;

	if (distance < 4.0f)
		distance = 4.0f;

	globe_width = 1.0f / distance;
	globe_height = 1.0f / distance;

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

	Rect rect = GUI::createRect(screen_x, screen_y, 0.1f, 0.1f);
	GUI::drawRect(rect, backgroudColor);

	rect = GUI::createRect(0.f, 0.f, 0.1f, 0.1f);
	GUI::drawRect(rect, obtainColorFromString("#FF0000FF"));

	rect = GUI::createRect(0.9f, 0.9f, 0.1f, 0.1f);
	GUI::drawRect(rect, obtainColorFromString("#00FF00FF"));
	//GUI::drawText(screen_x + percentLineHeight + percentLineHeight + marginForImage, screen_y + percentLineHeight, GImGui->Font, sizeFont, textColor, textToShow.c_str());
	ImGui::End();
#endif
}