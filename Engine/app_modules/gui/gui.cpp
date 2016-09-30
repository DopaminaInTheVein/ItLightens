#include "mcv_platform.h"
#include "gui.h"

#include "components/entity.h"
#include "components/entity_parser.h"
#include "components/comp_camera.h"
#include "comps/gui_cursor.h"
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
	dbg("GUI module started\n");

	return true;
}
//------------------------------------------------------------------------------------//

void CGuiModule::update(float dt)
{
	txtAction->render();

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
	dbg("GUI module stopped");
}

//------------------------------------------------------------------------------------//
CHandle CGuiModule::addGuiElement(std::string prefab, VEC3 pos, std::string tag, float scale)
{
	CHandle h = createPrefab(prefab);
	CHandle h_ui_cam = tags_manager.getFirstHavingTag("ui_camera");
	GET_COMP(ui_cam, h_ui_cam, TCompCamera);
	VEC3 min_ortho = ui_cam->getMinOrtho();
	VEC3 max_ortho = ui_cam->getMaxOrtho();
	VEC3 new_pos = min_ortho + (max_ortho - min_ortho) * pos;
	if (new_pos.z == 0.f) new_pos.z = 0.01f;
	GET_COMP(tmx, h, TCompTransform);
	tmx->setPosition(new_pos);
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