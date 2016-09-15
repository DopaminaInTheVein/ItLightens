#include "mcv_platform.h"
#include "gui_selector.h"

#include "components/entity.h"
#include "components/comp_transform.h"
#include "app_modules/gui/comps/gui_basic.h"

#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"

#include <math.h>
// Static info
map<string, statehandler> TCompGuiSelector::statemap = {};

// load Xml
bool TCompGuiSelector::load(MKeyValue& atts)
{
	return true;
}

// onCreate
void TCompGuiSelector::onCreate(const TMsgEntityCreated&) {
	options = vector<SelectorOption>();
	cur_option = 0;
	AddSelectorStates();
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnCreateGui, MY_NAME, MY_OWNER);
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
	}
}
void TCompGuiSelector::Over()
{
	if (!checkOver()) {
		ChangeState(STRING(Enabled));
	}
	else {
		// check left/right
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
	return res;
}

map<string, statehandler>* TCompGuiSelector::getStatemap() {
	return &statemap;
}