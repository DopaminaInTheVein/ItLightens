#include "mcv_platform.h"
#include "gui.h"

#include "components/entity.h"
#include "components/entity_parser.h"
#include "components/comp_camera.h"
#include "comps/gui_cursor.h"
#include "gui_munition.h"

// ImGui LIB headers
#pragma comment(lib, "imgui.lib" )

#include <Commdlg.h>

using namespace std;

stack<CHandle> CGuiModule::cursors = stack<CHandle>();

void CGuiModule::setCursorEnabled(bool enabled)
{
	if (cursors.size() > 0) {
		CHandle hcursor = cursors.top();
		if (hcursor.isValid()) {
			GET_COMP(cursor, hcursor, TCompGuiCursor);
			if (cursor) cursor->setEnabled(enabled);
		}
	}
}

// ------ External functions --------//
void CGuiModule::setActionAvailable(eAction action) {
	assert(txtAction);
	txtAction->setState(action);
}
bool CGuiModule::IsUiControl() const
{
	return ui_control;
}
bool * CGuiModule::IsUiControlPointer()
{
	return &ui_control;
}
void CGuiModule::SetUiControl(bool new_ui_control)
{
	ui_control = new_ui_control;
}

CHandle CGuiModule::getCursor()
{
	CHandle res;
	if (!cursors.empty()) res = cursors.top();
	return res;
}

void CGuiModule::pushCursor(CHandle h)
{
	cursors.push(h);
	ui_control = true;
}

// ----------------------------------- START MODULE ----------------------------------- //
bool CGuiModule::start()
{
	txtAction = new CGuiActionText(0.7f, 0.05f);
	munition = new CGuiMunition();
	initTextConfig();
	dbg("GUI module started\n");

	return true;
}
//------------------------------------------------------------------------------------//

void CGuiModule::update(float dt)
{
	txtAction->render();
	munition->update(dt);
	if (ui_control) {
		CHandle cursor;
		while (!cursor.isValid() && !cursors.empty()) {
			cursor = cursors.top();
			if (!cursor.isValid()) cursors.pop();
		}
		if (!cursor.isValid()) ui_control = false;
	}
}

// ----------------------------------- STOP MODULE ----------------------------------- //
void CGuiModule::stop() {
	delete txtAction;
	dbg("GUI module stopped");
}

//------------------------------------------------------------------------------------//

VEC3 CGuiModule::getWorldPos(VEC3 screen_pos)
{
	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();
	VEC3 new_pos = min_ortho + (max_ortho - min_ortho) * screen_pos;
	if (new_pos.z == 0.f) new_pos.z = 0.01f;
	return new_pos;
}

CHandle CGuiModule::addGuiElement(std::string prefab, VEC3 pos, std::string tag, float scale)
{
	CHandle h = createPrefab(prefab);

	GET_COMP(tmx, h, TCompTransform);
	tmx->setPosition(getWorldPos(pos));
	if (scale != 1.0f) {
		tmx->setScaleBase(VEC3(scale, scale, scale));
	}
	if (!tag.empty()) {
		TMsgSetTag msgTag;
		msgTag.add = true;
		msgTag.tag = tag;
		CEntity* e = h;
		e->sendMsg(msgTag);
	}

	dbg("gui_element created\n");
	return h;
}

void CGuiModule::moveGuiElement(CHandle h, VEC3 pos, float scale)
{
	if (h.isValid()) {
		GET_COMP(tmx, h, TCompTransform);
		if (tmx) {
			tmx->setScale(scale);
			tmx->setPosition(getWorldPos(pos));
		}
	}
}

VEC3 CGuiModule::getUiSize()
{
	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();
	return max_ortho - min_ortho;
}

// Get Screen Pos
VEC3 CGuiModule::getScreenPos(VEC3 pos)
{
	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();
	return (pos - min_ortho) / (max_ortho - min_ortho);
	/*
			CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
		GET_COMP(ui_cam, h_ui_cam, TCompCamera);
		VEC3 min_ortho = ui_cam->getMinOrtho();
		VEC3 max_ortho = ui_cam->getMaxOrtho();
		VEC3 new_pos = min_ortho + (max_ortho - min_ortho) * new_position;
		GET_COMP(tmx, handle, TCompTransform);
		tmx->setPosition(new_pos);
	*/
}

void CGuiModule::removeGuiElementByTag(std::string tag)
{
	CHandle handle = tags_manager.getFirstHavingTag(tag.c_str());
	if (handle.isValid()) { handle.destroy(); }
}

void CGuiModule::removeAllGuiElementsByTag(std::string tag)
{
	VHandles handles = tags_manager.getHandlesByTag(tag.c_str());
	for (auto handle : handles) { handle.destroy(); }
}

void CGuiModule::updateGuiElementPositionByTag(std::string tag, VEC3 new_position) {
	CHandle handle = tags_manager.getFirstHavingTag(tag.c_str());
	if (handle.isValid()) {
		CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
		GET_COMP(ui_cam, h_ui_cam, TCompCamera);
		VEC3 min_ortho = ui_cam->getMinOrtho();
		VEC3 max_ortho = ui_cam->getMaxOrtho();
		VEC3 new_pos = min_ortho + (max_ortho - min_ortho) * new_position;
		GET_COMP(tmx, handle, TCompTransform);
		tmx->setPosition(new_pos);
	}
}

#define FONT_JSON "./data/json/font.json"
using namespace Font;
//Font
void CGuiModule::initTextConfig()
{
	auto general = readIniAtrData(FONT_JSON, "general");
	auto space_values = readIniAtrData(FONT_JSON, "space_right");
	float size = general["size"];
	float default_space = 1.f - space_values["default"] / size;
	for (int i = 0; i < 256; i++) letter_sizes[i] = default_space;
	for (auto entry : space_values) {
		std::string entry_fixed = TextEncode::Utf8ToLatin1String(entry.first.c_str());
		if (entry_fixed.length() == 1) {
			unsigned char letter_char = entry_fixed.at(0);
			letter_sizes[letter_char] = entry.second / size;
		}
	}
	auto all_maps = readAllAtrMaps(FONT_JSON);
	for (auto entry : all_maps) {
		auto name = TextEncode::Utf8ToLatin1String(entry.first.c_str());
		if (name != "general" && name != "space_right") {
			int row = (int)entry.second["row"];
			int col = (int)entry.second["col"];
			int size_grid = (int)entry.second["size"];
			float rspace = entry.second["space_right"];
			float size_exact = 1 - size_grid + rspace / size;
			special_characters[name] = TCharacter(name, row - 1, col - 1, size_exact); // -1 because more use friendly (1-16, instead 0-15)
		}
	}
	initedText = true;
}