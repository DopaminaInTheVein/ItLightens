#include "mcv_platform.h"
#include "app_modules/imgui/module_imgui.h"
#include "comp_fading_globe.h"
#include "entity.h"
#include "app_modules/gui/gui.h"
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
#include "app_modules/gui/comps/gui_basic.h"

#include <math.h>

extern CShaderCte< TCteObject > shader_ctes_object;
extern CShaderCte< TCteCamera > shader_ctes_camera;
int TCompFadingGlobe::globes = 0;

using namespace std;

bool TCompFadingGlobe::load(MKeyValue& atts)
{
	prefab_route = atts.getString("route", "ui/effects/bafarada");
	globe_name = atts.getString("name", "char_globe" + to_string(globes++));
	//globe_name = atts.getString("name", "char_globe");
	float distance = atts.getFloat("dist", 1.0f);
	char_x = atts.getFloat("posx", 1.0f);
	char_y = atts.getFloat("posy", 1.0f);
	char_z = atts.getFloat("posz", 1.0f);
	screen_z = atts.getFloat("screen_z", 0.1f);
	ttl = atts.getFloat("ttl", 2.0f);
	MAX_DISTANCE = atts.getFloat("max_distance", -1.f);

	if (ttl <= 0.0f) {
		ttl = 0.0001f;
		perenne = true;
	}

	resolution_x = CApp::get().getXRes();
	resolution_y = CApp::get().getYRes();

	//if (prefab_route.find("bafarada") != string::npos)
	//	char_y += 2.0f - (1.f / distance);

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
	//screen_z = 0.75f;

	if (!isBehindCamera() && inDistance()) {
		createGlobe();
		//globe_handle = Gui->addGuiElement(prefab_route, VEC3(screen_x, 1.f - screen_y, screen_z), globe_name);
		//added = true;
	}

	//if (distance < 4.0f)
		//distance = 4.0f;

	//globe_width = 1.0f / distance;
	//globe_height = 1.0f / distance;

	return true;
}

bool TCompFadingGlobe::getUpdateInfo() {
	if (!camera_main.isValid()) camera_main = tags_manager.getFirstHavingTag("camera_main");
	cam = GETH_COMP(camera_main, TCompCameraMain);
	cam_tmx = GETH_COMP(camera_main, TCompTransform);
	if (!cam || !cam_tmx) return false;

	return true;
}

void TCompFadingGlobe::update(float dt) {
	static float accumTime = 0.0f;

	// update screen_x and screen_y when camera moves

	float4 proj_coords = mul(VEC4(char_x, char_y, char_z, 1.0f), shader_ctes_camera.ViewProjection);
	proj_coords /= proj_coords.z;

	screen_x = ((proj_coords.x + 1.0f) / 2.0f);
	screen_y = ((1.f - proj_coords.y) / 2.0f);

	if (!globe_handle.isValid() && !isBehindCamera() && inDistance()) {
		createGlobe();
	}
	else if (globe_handle.isValid() && !isBehindCamera() && inDistance()) {
		Gui->moveGuiElement(globe_handle, VEC3(screen_x, 1.f - screen_y, screen_z), getGlobeScale());
	}
	else {
		//Gui->removeGuiElementByTag(globe_name);
		globe_handle.destroy();
		globe_handle = CHandle();
		//added = false;
	}

	if (globe_handle.isValid()) {
		GET_COMP(globe_tmx, globe_handle, TCompTransform);
		globe_tmx->setScale(getGlobeScale());
	}
	// time to life control for the globe

	if (ttl >= 0.0f && !perenne) {
		ttl -= getDeltaTime();
	}
	else if (ttl < 0.0f) {
		//textureIcon->destroy();
		CHandle h = CHandle(this).getOwner();
		h.destroy();

		// clean instance of the globe
		//Gui->removeGuiElementByTag(globe_name);
		globe_handle.destroy();
	}
}

void TCompFadingGlobe::createGlobe()
{
	globe_handle = Gui->addGuiElement(prefab_route, VEC3(screen_x, 1.f - screen_y, screen_z), globe_name);
	GET_COMP(gui, globe_handle, TCompGui);
	if (gui) size_world = gui->GetSizeWorld();
	else size_world = -1.f;
}

float TCompFadingGlobe::getGlobeScale()
{
	if (size_world < 0.f) return 1.f;
	float dist = realDist(VEC3(char_x, char_y, char_z), cam_tmx->getPosition());
	float range_x = sinf(cam->getFov())*dist*2.f;
	return Gui->getUiSize().x * size_world / range_x;
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

bool TCompFadingGlobe::isBehindCamera() {
	float cam_x = shader_ctes_camera.CameraWorldPos.x;
	float cam_y = shader_ctes_camera.CameraWorldPos.y;
	float cam_z = shader_ctes_camera.CameraWorldPos.z;

	float cam_lookat_x = shader_ctes_camera.CameraFront.x;
	float cam_lookat_y = shader_ctes_camera.CameraFront.y;
	float cam_lookat_z = shader_ctes_camera.CameraFront.z;

	return ((char_x - cam_x) * cam_lookat_x
		+ (char_y - cam_y) * cam_lookat_y
		+ (char_z - cam_z) * cam_lookat_z
		< 0);
}

bool TCompFadingGlobe::inDistance() {
	if (MAX_DISTANCE < 0.f)
		return true;

	VEC3 camera_pos = shader_ctes_camera.CameraWorldPos;
	float dist = simpleDist(camera_pos, VEC3(char_x, char_y, char_z));

	return dist <= MAX_DISTANCE;
}