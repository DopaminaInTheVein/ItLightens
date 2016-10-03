#include "mcv_platform.h"
#include "gui_drag.h"

#include "components/entity.h"
#include "components/entity_parser.h"

#include "components/comp_transform.h"
#include "components/comp_text.h"
#include "app_modules/gui/gui.h"
#include "app_modules/gui/comps/gui_basic.h"

#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"

#include <math.h>

#define LEFT_EVENT "left_event"
#define RIGHT_EVENT "right_event"
#define DRAG_ITEM_PREFAB "ui/drag_item"

// Static info
map<string, statehandler> TCompGuiDrag::statemap = {};

// load Xml
bool TCompGuiDrag::load(MKeyValue& atts)
{
	return true;
}

// onCreate
void TCompGuiDrag::onCreate(const TMsgEntityCreated&) {
	getUpdateInfo();
	value = last_value = .5f;
	AddDragStates();
	my_pos = myTransform->getPosition();
	AddDragItem();
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCreateGui, MY_NAME, MY_OWNER);
	// Add side arrows
}

bool TCompGuiDrag::getUpdateInfo()
{
	myGui = GETH_MY(TCompGui);
	if (!myGui) return false;

	myTransform = GETH_MY(TCompTransform);
	if (!myTransform) return false;

	cursor = myGui->getCursor();
	if (!cursor.isValid()) return false;

	cursorTransform = GETH_COMP(cursor, TCompTransform);
	if (!cursorTransform) return false;

	if (!drag_item.isValid()) return false;

	dragTransform = GETH_COMP(drag_item, TCompTransform);
	if (!dragTransform) return false;

	dragGui = GETH_COMP(drag_item, TCompGui);
	if (!dragGui) return false;

	return true;
}

void TCompGuiDrag::update(float dt)
{
	Recalc();
	updateRenderState();
}

void TCompGuiDrag::AddDragItem()
{
	drag_item = createPrefab(DRAG_ITEM_PREFAB);
	GET_COMP(tmx, drag_item, TCompTransform);
	GET_COMP(gui, drag_item, TCompGui);
	if (tmx && gui) {
		tmx->setPosition(myTransform->getPosition() + VEC3_FRONT * 0.01f);
		float height = myGui->GetHeight();
		tmx->setScaleBase(VEC3(height, height, height));
		gui->SetHeight(height);
		gui->SetWidth(height); // No es un error, la flecha es cuadrada con la altura del selector
		gui->SetParent(MY_OWNER);
	}
}

#define AddDragState(name) AddState(STRING(name), (statehandler)&TCompGuiDrag::name)
#define SetDragState(name) ChangeState(STRING(name))
void TCompGuiDrag::AddDragStates()
{
	if (statemap.empty()) {
		AddDragState(Disabled);
		AddDragState(Enabled);
		AddDragState(Over);
		AddDragState(Clicked);
	}
	SetDragState(Enabled);
}

void TCompGuiDrag::Disabled()
{
	//Check Enabled
	if (myGui->IsEnabled()) {
		enable();
	}
	else render_state = RSTATE_DISABLED;
}
void TCompGuiDrag::Enabled()
{
	if (checkEnabled()) {
		if (checkOver() && !controller->IsGuiItemDragged()) {
			ChangeState(STRING(Over));
			notifyOver(true);
		}
		else render_state = RSTATE_ENABLED;
	}
}
void TCompGuiDrag::Over()
{
	if (checkEnabled()) {
		if (checkOver()) {
			if (controller->IsGuiItemDragged()) {
				SetDragState(Clicked);
			}
			else render_state = RSTATE_OVER;
			if (controller->IsLeftPressed()) {
				SetValue(value - 0.1f);
				notifyValue();
			}
			else if (controller->IsRightPressed()) {
				SetValue(value + 0.1f);
				notifyValue();
			}
		}
		else {
			ChangeState(STRING(Enabled));
			notifyOver(false);
		}
	}
}
void TCompGuiDrag::Clicked()
{
	if (checkEnabled()) {
		if (controller->IsGuiItemDragged()) {
			render_state = RSTATE_OVER;
			updateDrag();
		}
		else {
			notifyValue();
			SetDragState(Over);
		}
	}
}

void TCompGuiDrag::updateDrag()
{
	VEC3 new_pos = my_pos;
	new_pos.x = cursorTransform->getPosition().x;
	float w = myGui->GetWidth();
	float w_half = w / 2.f;
	clamp_me(new_pos.x, my_pos.x - w_half, my_pos.x + w_half);
	dragTransform->setPosition(new_pos);
	value = .5f + (new_pos.x - my_pos.x) / w;
	if (abs(last_value - value) > 0.001f) {
		notifyValue();
	}
}

void TCompGuiDrag::notifyValue()
{
	char params[256];
	sprintf(params, "\"%s\", %f", MY_NAME, value);
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnValueChanged, std::string(params), MY_OWNER);
	last_value = value;
}

bool TCompGuiDrag::checkOver()
{
	VEC3 cursorPos = cursorTransform->getPosition();
	VEC3 delta = my_pos - cursorPos;
	return abs(delta.x) < myGui->GetWidth()*0.5f && abs(delta.y) < myGui->GetHeight()*0.5f;
}
void TCompGuiDrag::notifyOver(bool over)
{
	is_over = over;

	// Cursor Message (for something?)
	TMsgOverButton msg;
	msg.button = CHandle(this).getOwner();
	msg.is_over = over;
	cursor.sendMsg(msg);

	if (over) {
		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnMouseOver, MY_NAME, MY_OWNER);
	}
	else {
		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnMouseUnover, MY_NAME, MY_OWNER);
	}
}
void TCompGuiDrag::renderInMenu()
{
	IMGUI_SHOW_STRING(aicontroller::getState());
	IMGUI_SHOW_FLOAT(value);
}

void TCompGuiDrag::SetValue(float new_value)
{
	getUpdateInfo();
	value = last_value = clamp(new_value, 0.f, 1.f);
	VEC3 new_pos = my_pos + VEC3_FRONT * 0.01f;
	new_pos.x += (value - 0.5f) * myGui->GetWidth();
	if (dragTransform) dragTransform->setPosition(new_pos);
}

//void TCompGuiDrag::onGuiNotify(const TMsgGuiNotify& msg)
//{
//	if (options.size() == 0) {
//		assert(false);
//		return;
//	}
//	int new_id = 0;
//	if (msg.event_name == LEFT_EVENT) {
//		new_id = cur_option - 1;
//	}
//	else if (msg.event_name == RIGHT_EVENT) {
//		new_id = cur_option + 1;
//	}
//	mod(new_id, options.size());
//	SelectOption(new_id);
//	char param[64];
//	sprintf(param, "\"%s\", %d", MY_NAME, cur_option);
//	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnChoose, string(param), MY_OWNER);
//}

bool TCompGuiDrag::checkEnabled()
{
	bool is_enabled = myGui->IsEnabled();
	if (!is_enabled) {
		disable();
	}
	return is_enabled;
}
void TCompGuiDrag::enable()
{
	ChangeState(STRING(Enabled));
	dragGui->SetEnabled(true);
}
void TCompGuiDrag::disable()
{
	ChangeState(STRING(Disabled));
	dragGui->SetEnabled(false);
}

void TCompGuiDrag::updateRenderState()
{
	myGui->setRenderTarget(render_state, 2.f);
	dragGui->setRenderTarget(render_state, 2.f);
}

map<string, statehandler>* TCompGuiDrag::getStatemap() {
	return &statemap;
}