#include "mcv_platform.h"
#include "gui_selector.h"

#include "components/entity.h"
#include "components/entity_parser.h"

#include "components/comp_transform.h"
#include "app_modules/gui/comps/gui_basic.h"

#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"

#include <math.h>

#define LEFT_EVENT "left_event"
#define RIGHT_EVENT "right_event"

// Static info
map<string, statehandler> TCompGuiSelector::statemap = {};

// load Xml
bool TCompGuiSelector::load(MKeyValue& atts)
{
	return true;
}

// onCreate
void TCompGuiSelector::onCreate(const TMsgEntityCreated&) {
	getUpdateInfo();
	options = vector<SelectorOption>();
	cur_option = 0;
	AddSelectorStates();
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCreateGui, MY_NAME, MY_OWNER);

	// Add side arrows
	AddArrows();
}

void TCompGuiSelector::AddArrows()
{
	float offset_pos = 0.5f * (myGui->GetWidth() + myGui->GetHeight());
	AddArrow(arrow_left, "ui/selector_left", LEFT_EVENT, -offset_pos);
	AddArrow(arrow_right, "ui/selector_right", RIGHT_EVENT, +offset_pos);
}

void TCompGuiSelector::AddArrow(CHandle& h, string prefab, string name_event, float offset_pos)
{
	h = createPrefab(prefab);
	GET_COMP(tmx, h, TCompTransform);
	GET_COMP(gui, h, TCompGui);
	if (tmx && gui) {
		tmx->setPosition(myTransform->getPosition() + offset_pos * VEC3_RIGHT);
		float height = myGui->GetHeight();
		tmx->setScaleBase(VEC3(height, height, height));
		gui->SetHeight(height);
		gui->SetWidth(height); // No es un error, la flecha es cuadrada con la altura del selector
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
	//Nothing to do
}
void TCompGuiSelector::Enabled()
{
	//checkOver --> Over
	if (checkOver()) {
		ChangeState(STRING(Over));
		notifyOver(true);
	}
}
void TCompGuiSelector::Over()
{
	if (!checkOver()) {
		ChangeState(STRING(Enabled));
		notifyOver(false);
		//options[cur_option].color = COLOR_NOT_SELECTED;
	}
	else {
		if (controller->IsLeftPressed()) {
			arrow_left.sendMsg(TMsgClicked());
		}
		else if (controller->IsRightPressed()) {
			arrow_right.sendMsg(TMsgClicked());
		}
	}
}

bool TCompGuiSelector::getUpdateInfo()
{
	myTransform = GETH_MY(TCompTransform);
	if (!myTransform) return false;

	cursor = TCompGui::getCursor();
	if (!cursor.isValid()) return false;

	cursorTransform = GETH_COMP(cursor, TCompTransform);
	if (!cursorTransform) return false;

	myGui = GETH_MY(TCompGui);
	if (!myGui) return false;

	return true;
}

void TCompGuiSelector::update(float dt)
{
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
		ImGui::Text("Option %d: %s", i++, sel_opt.text.c_str());
	}
}

int TCompGuiSelector::AddOption(string option)
{
	int res = options.size();
	SelectorOption sel_option;
	sel_option.text = option;
	options.push_back(sel_option);
	//TODO: create text options[res]
	//TODO: if res == options[res].color = COLOR_SELECTED
	//TODO: else options[res].alfa = 0
	return res;
}

void TCompGuiSelector::onGuiNotify(const TMsgGuiNotify& msg)
{
	int new_id = 0;
	if (msg.event_name == LEFT_EVENT) {
		new_id = cur_option - 1;
	}
	else if (msg.event_name == RIGHT_EVENT) {
		new_id = cur_option + 1;
	}
	new_id %= options.size();
	SelectOption(new_id);
	char param[64];
	sprintf(param, "\"%s\", %d", MY_NAME, cur_option);
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnChoose, string(param), MY_OWNER);
}

void TCompGuiSelector::SelectOption(int id)
{
	//options[cur_option] --> alfa = 0;
	cur_option = id;
	//options[cur_option=id].color = COLOR_SELECTED;
}

TCompGuiSelector::~TCompGuiSelector()
{
	arrow_left.destroy();
	arrow_right.destroy();
}

map<string, statehandler>* TCompGuiSelector::getStatemap() {
	return &statemap;
}