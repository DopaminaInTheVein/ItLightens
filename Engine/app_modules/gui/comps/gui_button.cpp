#include "mcv_platform.h"
#include "gui_button.h"

#include "components/entity.h"
#include "components/comp_transform.h"
#include "app_modules/gui/comps/gui_basic.h"

#include "app_modules/gameController.h"
#include "app_modules/io/io.h"
#include "app_modules/logic_manager/logic_manager.h"

#include <math.h>
// Static info
map<string, statehandler> TCompGuiButton::statemap = {};

bool TCompGuiButton::options_loaded = false;
float * TCompGuiButton::speeds_increase = nullptr;
float * TCompGuiButton::speeds_decrease = nullptr;
float TCompGuiButton::t_enabled;
float TCompGuiButton::t_disabled;
float TCompGuiButton::t_over;
float TCompGuiButton::t_unover;
float TCompGuiButton::t_clicked;
float TCompGuiButton::t_unclicked;
float TCompGuiButton::t_released;
float TCompGuiButton::t_unreleased;

//#define SetSpeedButtonState(sufix) sp_##sufix = 1.f / t_##sufix;
void TCompGuiButton::loadOptions()
{
	// Check if options are already loaded
	if (options_loaded) return;
	options_loaded = true;

	//Read from file_ini
	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr_json;
	map<std::string, float> fields_base = readIniAtrData(file_ini, "gui_button");
	assignValueToVar(t_enabled, fields_base);
	assignValueToVar(t_disabled, fields_base);
	assignValueToVar(t_over, fields_base);
	assignValueToVar(t_unover, fields_base);
	assignValueToVar(t_clicked, fields_base);
	assignValueToVar(t_unclicked, fields_base);
	assignValueToVar(t_released, fields_base);
	assignValueToVar(t_unreleased, fields_base);

	speeds_increase = new float[RStates::SIZE];
	speeds_decrease = new float[RStates::SIZE];
	speeds_increase[RStates::DISABLED] = inverseFloat(t_enabled);
	speeds_increase[RStates::ENABLED] = inverseFloat(t_over);
	speeds_increase[RStates::OVER] = inverseFloat(t_clicked);
	speeds_increase[RStates::CLICKED] = inverseFloat(t_released);
	speeds_increase[RStates::RELEASED] = 0.f;

	speeds_decrease[RStates::DISABLED] = 0.f;
	speeds_decrease[RStates::ENABLED] = inverseFloat(t_disabled);
	speeds_decrease[RStates::OVER] = inverseFloat(t_unover);
	speeds_decrease[RStates::CLICKED] = inverseFloat(t_unclicked);
	speeds_decrease[RStates::RELEASED] = inverseFloat(t_unreleased);
}

// load Xml
bool TCompGuiButton::load(MKeyValue& atts)
{
	loadOptions();
	width = atts.getFloat("width", 0.f);
	height = atts.getFloat("height", 0.f);
	init_enabled = atts.getBool("enabled", true);
	return true;
}

// onCreate
#define AddButtonState(name) AddState(STRING(name), (statehandler)&TCompGuiButton::name);
void TCompGuiButton::onCreate(const TMsgEntityCreated&) {
	AddButtonStates();
	render_state = render_state_target = 0.f;
	init_enabled ? ChangeState(STRING(Enabled)) : ChangeState(STRING(Disabled));
}

void TCompGuiButton::AddButtonStates()
{
	if (statemap.empty()) {
		AddButtonState(Disabled);
		AddButtonState(Enabled);
		AddButtonState(Over);
		AddButtonState(Clicked);
		AddButtonState(Released);
		AddButtonState(Actioned);
	}
}

void TCompGuiButton::Disabled()
{
	//Nothing to do
}
void TCompGuiButton::Enabled()
{
	//checkOver --> Over
	if (checkOver()) {
		ChangeState(STRING(Over));
		render_state_target = RSTATE_OVER;
		notifyOver(true);
	}
}
void TCompGuiButton::Over()
{
	if (!checkOver()) {
		ChangeState(STRING(Enabled));
		render_state_target = RSTATE_ENABLED;
		notifyOver(false);
	}
	else if (checkClicked()) {
		ChangeState(STRING(Clicked));
		render_state = RSTATE_OVER;
		render_state_target = RSTATE_CLICKED;
		logic_manager->throwEvent(CLogicManagerModule::EVENT::OnButtonPressed, MY_NAME, MY_OWNER);
	}
}
void TCompGuiButton::Clicked()
{
	if (!checkOver()) {
		ChangeState(STRING(Enabled));
		render_state_target = RSTATE_ENABLED;
		notifyOver(false);
	}
	else if (checkReleased()) {
		ChangeState(STRING(Released));
		render_state = RSTATE_CLICKED;
		render_state_target = RSTATE_RELEASED;
		//logic_manager->throwEvent(CLogicManagerModule::EVENT::OnClicked, MY_NAME, MY_OWNER);
	}
}
void TCompGuiButton::Released()
{
	if (render_state >= RSTATE_RELEASED) {
		ChangeState(STRING(Actioned));
	}
}
void TCompGuiButton::Actioned()
{
	onClick(TMsgClicked());
	ChangeState(STRING(Over));
	render_state_target = RSTATE_OVER;
}

bool TCompGuiButton::getUpdateInfo()
{
	myTransform = GETH_MY(TCompTransform);
	if (!myTransform) return false;

	cursor = tags_manager.getFirstHavingTag("gui_cursor");
	if (!cursor.isValid()) return false;

	cursorTransform = GETH_COMP(cursor, TCompTransform);
	if (!cursorTransform) return false;

	myGui = GETH_MY(TCompGui);
	if (!myGui) return false;

	return true;
}

void TCompGuiButton::update(float dt)
{
	Recalc();
	updateRenderState();
	updateSize();
}

void TCompGuiButton::onClick(const TMsgClicked&)
{
	logic_manager->throwEvent(CLogicManagerModule::EVENT::OnClicked, MY_NAME, MY_OWNER);
}

void TCompGuiButton::updateRenderState()
{
	// Calc speed
	float speed = 0.f;
	render_state = myGui->getRenderState();
	if (render_state_target > render_state) {
		speed = speeds_increase[(int)(floor(render_state)) + 1];
	}
	else if (render_state_target < render_state) {
		speed = speeds_decrease[(int)(ceil(render_state)) + 1];
	}

	//Apply spped
	myGui->setRenderTarget(render_state_target, speed);
}

void TCompGuiButton::updateSize()
{
	//update size buttons
	float offset = render_state;

	//reset offset
	if (offset > RSTATE_OVER) {
		offset = (RSTATE_RELEASED - offset) / RSTATE_RELEASED;
	}
	// +1 because default render state is 0
	float value = 1 + offset*0.25f;
	if (value != 0)
		myTransform->setScale(VEC3(value, value, value));
}

#define DragFloatTimes(name, sufix, rstate) if (ImGui::DragFloat(STRING(name), &name, 0.01f, 0.001f, 1.f)) speeds_##sufix[RStates::rstate] = inverseFloat(name);
void TCompGuiButton::renderInMenu()
{
	IMGUI_SHOW_STRING(getState());
	IMGUI_SHOW_FLOAT(render_state);
	DragFloatTimes(t_enabled, increase, DISABLED);
	DragFloatTimes(t_over, increase, ENABLED);
	DragFloatTimes(t_clicked, increase, OVER);
	DragFloatTimes(t_released, increase, CLICKED);

	DragFloatTimes(t_disabled, decrease, ENABLED);
	DragFloatTimes(t_unover, decrease, OVER);
	DragFloatTimes(t_unclicked, decrease, CLICKED);
	DragFloatTimes(t_unreleased, decrease, RELEASED);
}

bool TCompGuiButton::checkOver()
{
	VEC3 myPos = myTransform->getPosition();
	VEC3 cursorPos = cursorTransform->getPosition();
	VEC3 delta = myPos - cursorPos;
	return abs(delta.x) < width*0.5f && abs(delta.y) < height*0.5f;
}

bool TCompGuiButton::checkClicked()
{
	return controller->ActionButtonBecomesPessed();
}

bool TCompGuiButton::checkReleased()
{
	return controller->IsActionButtonReleased();
}

void TCompGuiButton::notifyOver(bool over)
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

map<string, statehandler>* TCompGuiButton::getStatemap() {
	return &statemap;
}