#include "mcv_platform.h"
#include "gui_selector.h"

#include "components/entity.h"
#include "components/entity_parser.h"

#include "components/comp_transform.h"
#include "components/comp_text.h"
#include "app_modules/gui/gui.h"
#include "app_modules/gui/comps/gui_basic.h"

#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"
#include "app_modules/lang_manager/lang_manager.h"

#include <math.h>

#define LEFT_EVENT "left_event"
#define RIGHT_EVENT "right_event"
#define COLOR_SELECTED "#0000FFFF"
#define COLOR_NORMAL "#FFFFFFFF"
#define COLOR_HIDDEN "#00000000"
#define COLOR_DISABLED "#757575FF"
#define COLOR_SPEED 0.3f

// Static info
map<string, statehandler> TCompGuiSelector::statemap = {};

// load Xml
bool TCompGuiSelector::load(MKeyValue& atts)
{
	return true;
}

// onCreate
void TCompGuiSelector::onCreate() {
	getUpdateInfo();
	options = VHandles();
	cur_option = 0;
	AddSelectorStates();
	my_pos = myTransform->getPosition();
	AddArrows();
	//logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCreateGui, MY_NAME, MY_OWNER);
	// Add side arrows
}

void TCompGuiSelector::AddArrows()
{
	float offset_pos = 0.5f * (myGui->GetWidth() - myGui->GetHeight());
	AddArrow(arrow_left, "ui/selector_left", LEFT_EVENT, -offset_pos);
	AddArrow(arrow_right, "ui/selector_right", RIGHT_EVENT, +offset_pos);
}

void TCompGuiSelector::AddArrow(CHandle& h, string prefab, string name_event, float offset_pos)
{
	h = createPrefab(prefab);
	GET_COMP(tmx, h, TCompTransform);
	GET_COMP(gui, h, TCompGui);
	if (tmx && gui) {
		tmx->setPosition(myTransform->getPosition() + offset_pos * VEC3_RIGHT + VEC3_FRONT * 0.01f);
		float height = myGui->GetHeight();
		tmx->setScaleBase(VEC3(height, height, height));
		gui->SetHeight(height);
		gui->SetWidth(height); // No es un error, la flecha es cuadrada con la altura del selector
		gui->SetParent(MY_OWNER);
	}
	TMsgGuiSetListener msg;
	msg.event_name = name_event;
	msg.listener = MY_OWNER;
	h.sendMsg(msg);
}

#define AddSelectorState(name) AddState(STRING(name), (statehandler)&TCompGuiSelector::name);
void TCompGuiSelector::AddSelectorStates()
{
	if (statemap.empty()) {
		AddSelectorState(Disabled);
		AddSelectorState(Enabled);
		AddSelectorState(Over);
	}
	ChangeState(STRING(Enabled));
}

void TCompGuiSelector::Disabled()
{
	//Check Enabled
	if (myGui->IsEnabled()) {
		enable();
	}
}
void TCompGuiSelector::Enabled()
{
	if (checkEnabled()) {
		//checkOver --> Over
		if (checkOver()) {
			ChangeState(STRING(Over));
			notifyOver(true);
		}
	}
}
void TCompGuiSelector::Over()
{
	if (checkEnabled()) {
		if (!checkOver()) {
			ChangeState(STRING(Enabled));
			notifyOver(false);
		}
		else {
			if (controller->IsLeftPressedSelector()) {
				arrow_left.sendMsg(TMsgClicked());
			}
			else if (controller->IsRightPressedSelector()) {
				arrow_right.sendMsg(TMsgClicked());
			}
		}
	}
}

bool TCompGuiSelector::getUpdateInfo()
{
	myGui = GETH_MY(TCompGui);
	if (!myGui) return false;

	myTransform = GETH_MY(TCompTransform);
	if (!myTransform) return false;

	cursor = myGui->getCursor();
	if (!cursor.isValid()) return false;

	cursorTransform = GETH_COMP(cursor, TCompTransform);
	if (!cursorTransform) return false;

	return true;
}

void TCompGuiSelector::update(float dt)
{
	updateRenderState();
	Recalc();
}

bool TCompGuiSelector::checkOver()
{
	VEC3 myPos = myTransform->getPosition();
	VEC3 cursorPos = cursorTransform->getPosition();
	VEC3 delta = myPos - cursorPos;
	return abs(delta.x) < myGui->GetWidth()*0.5f && abs(delta.y) < myGui->GetHeight()*0.5f;
}
void TCompGuiSelector::notifyOver(bool over)
{
	is_over = over;
	GET_COMP(txt, options[cur_option], TCompText);
	txt->SetColorTarget(over ? COLOR_SELECTED : COLOR_NORMAL, COLOR_SPEED);

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
void TCompGuiSelector::renderInMenu()
{
	IMGUI_SHOW_STRING(aicontroller::getState());
	IMGUI_SHOW_INT(cur_option);
	int i = 0;
	for (auto sel_opt : options) {
		GET_COMP(txt, sel_opt, TCompText);
		//ImGui::Text("Option %d: %s", i++, txt ? txt->getText() : "???");
	}
}

int TCompGuiSelector::AddOption(string option)
{
	getUpdateInfo();
	int res = options.size();

	CHandle h_option = createPrefab("ui/text");
	GET_COMP(txt, h_option, TCompText);
	VEC3 postxt = myTransform->getPosition();
	float w = myGui->GetWidth();
	float h = myGui->GetHeight();
	postxt += VEC3(-w / 2.f + h*1.25f, -h / 2.f, .0001f);
	txt->SetText(lang_manager->getText(option, "title"));
	txt->SetSize(myGui->GetHeight());
	txt->SetPosWorld(postxt);
	options.push_back(h_option);
	setTextVisible(res, res == 0);
	GET_COMP(gui, h_option, TCompGui);
	gui->SetParent(MY_OWNER);
	return res;
}

void TCompGuiSelector::onGuiNotify(const TMsgGuiNotify& msg)
{
	getUpdateInfo();
	if (options.size() == 0) {
		assert(false);
		return;
	}
	int new_id = 0;
	if (msg.event_name == LEFT_EVENT) {
		new_id = cur_option - 1;
	}
	else if (msg.event_name == RIGHT_EVENT) {
		new_id = cur_option + 1;
	}
	mod(new_id, options.size());
	SelectOption(new_id);
	char param[64];
	sprintf(param, "\"%s\", %d", MY_NAME, cur_option);
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnChoose, string(param), MY_OWNER);
}

//void TCompGuiSelector::onLanguageChanged(const TMsgLanguageChanged& msg)
//{
//	for (auto opt : options) opt.destroy();
//	options.clear();
//	cur_option = 0;
//	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCreateGui, MY_NAME, MY_OWNER);
//}

void TCompGuiSelector::SelectOption(int id)
{
	getUpdateInfo();
	setTextVisible(cur_option, false);
	cur_option = clamp(id, 0, options.size());
	assert(id == cur_option);
	setTextVisible(cur_option = id, true);
}

TCompText* TCompGuiSelector::setTextVisible(int option, bool visible)
{
	CHandle opt = options[option];
	GET_COMP(txt, opt, TCompText);
	if (txt) {
		if (visible) {
			txt->SetColorTarget(is_over ? COLOR_SELECTED : COLOR_NORMAL, COLOR_SPEED);
		}
		else {
			txt->SetColor(COLOR_HIDDEN);
		}
		//txt->SetZ(my_pos.z + ((visible ? 1 : -1) * 0.05f));
	}
	return txt;
}

bool TCompGuiSelector::checkEnabled()
{
	bool is_enabled = myGui->IsEnabled();
	if (!is_enabled) {
		disable();
	}
	return is_enabled;
}
void TCompGuiSelector::enable()
{
	ChangeState(STRING(Enabled));

	//Text Color, buttons...
	GET_COMP(txt, options[cur_option], TCompText);
	txt->SetColorTarget(is_over ? COLOR_SELECTED : COLOR_NORMAL);

	GET_COMP(larrow_gui, arrow_left, TCompGui);
	GET_COMP(rarrow_gui, arrow_right, TCompGui);
	larrow_gui->SetEnabled(true);
	rarrow_gui->SetEnabled(true);
}
void TCompGuiSelector::disable()
{
	ChangeState(STRING(Disabled));

	//Text Color, buttons...
	GET_COMP(txt, options[cur_option], TCompText);
	txt->SetColorTarget(COLOR_DISABLED);

	GET_COMP(larrow_gui, arrow_left, TCompGui);
	GET_COMP(rarrow_gui, arrow_right, TCompGui);
	larrow_gui->SetEnabled(false);
	rarrow_gui->SetEnabled(false);
}

void TCompGuiSelector::updateRenderState()
{
	if (myGui->IsEnabled()) myGui->setRenderTarget(0.f, 2.f);
	else myGui->setRenderTarget(-1.f, 2.f);
}

map<string, statehandler>* TCompGuiSelector::getStatemap() {
	return &statemap;
}