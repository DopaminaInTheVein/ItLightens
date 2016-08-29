#include "mcv_platform.h"
#include "app_modules/imgui/module_imgui.h"
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
	ttl = 2.0f;
	globe_name = atts.getString("name", "char_globe");
	distance = atts.getFloat("dist", 1.0f);
	char_x = atts.getFloat("posx", 1.0f);
	char_y = atts.getFloat("posy", 1.0f);
	char_z = atts.getFloat("posz", 1.0f);

	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();

	char_y += 1.5f - (1.f / distance);

	// First option: computing manually the projective space coords

	/*float aspect_ratio = shader_ctes_camera.CameraAspectRatio;
	float zfar = shader_ctes_camera.CameraZFar;
	float znear = shader_ctes_camera.CameraZNear;

	float ys = 1.0f / shader_ctes_camera.CameraTanHalfFov;
	float xs = ys / aspect_ratio;

	float proj_x = char_x * xs;
	float proj_y = char_y * ys;
	float proj_z = char_z * (zfar / (zfar - znear)) - (zfar*znear / (zfar - znear));

	float4 proj_coords = float4(proj_x, proj_y, proj_z, char_z);
	proj_coords = proj_coords / proj_z;

	screen_x = ((proj_coords.x + 1.0f) / 2.0f);
	screen_y = ((1.f - proj_coords.y) / 2.0f);*/

	// Second option: using camera viewprojection matrix

	float4 proj_coords = mul(VEC4(char_x, char_y, char_z, 1.0f), shader_ctes_camera.ViewProjection);
	proj_coords /= proj_coords.z;

	screen_x = ((proj_coords.x + 1.0f) / 2.0f);
	screen_y = ((1.f - proj_coords.y) / 2.0f);

	if (!added) {
		Gui->addGuiElement("ui/bafarada", VEC3(screen_x, 1.f - screen_y, 0.f), globe_name);
		added = true;
	}

	if (distance < 4.0f)
		distance = 4.0f;

	globe_width = 1.0f / distance;
	globe_height = 1.0f / distance;

	return true;
}

void TCompFadingGlobe::update(float dt) {
	static float accumTime = 0.0f;

	// update screen_x and screen_y when camera moves

	float4 proj_coords = mul(VEC4(char_x, char_y, char_z, 1.0f), shader_ctes_camera.ViewProjection);
	proj_coords /= proj_coords.z;

	screen_x = ((proj_coords.x + 1.0f) / 2.0f);
	screen_y = ((1.f - proj_coords.y) / 2.0f);

	if (added) {
		Gui->updateGuiElementPositionByTag(globe_name, VEC3(screen_x, 1.f - screen_y, 0.f));
	}

	if (ttl >= 0.0f) {
		ttl -= getDeltaTime();
	}
	else {
		//textureIcon->destroy();
		CHandle h = CHandle(this).getOwner();
		h.destroy();

		// clean instance of the globe
		Gui->removeGuiElementByTag(globe_name);
	}
}

void TCompFadingGlobe::render() const {
#ifndef NDEBUG
	PROFILE_FUNCTION("TCompFadingGlobe render");

	bool b = false;

	ImGui::Begin("Game GUI", &b, ImVec2(resolution_x, resolution_y), 0.0f, flags);
	ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));

	/*Rect rect = GUI::createRect(screen_x, screen_y, 0.1f, 0.1f);
	GUI::drawRect(rect, obtainColorFromString("#00FF00FF"));*/

	ImGui::End();
#endif
}