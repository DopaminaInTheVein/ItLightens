#include "mcv_platform.h"
#include "gui_cursor.h"

#include "components/entity.h"
#include "components/comp_transform.h"
#include "components/comp_camera.h"

#include "app_modules/gameController.h"
#include "app_modules/io/io.h"
#include "gui_basic.h"

// load Xml
bool TCompGuiCursor::load(MKeyValue& atts)
{
	GameController->SetUiControl(true);
	GameController->SetGameState(CGameController::STOPPED);

	speed = atts.getFloat("speed", 0.01f);
	menu_name = atts.getString("menu_name", "");
	x = y = -1;
	init_x = atts.getInt("init_x", 0);
	init_y = atts.getInt("init_y", 0);
	return true;
}

void TCompGuiCursor::onCreate(const TMsgEntityCreated&)
{
	TCompGui::pushCursor(MY_OWNER);
	//GameController->SetUiControl(true);
}

bool TCompGuiCursor::getUpdateInfo()
{
	myGui = GETH_MY(TCompGui);
	if (!myGui) return false;

	myTransform = GETH_MY(TCompTransform);
	if (!myTransform) return false;

	if (!ui_camera_h.isValid()) {
		ui_camera_h = tags_manager.getFirstHavingTag("ui_camera");
		if (!ui_camera_h.isValid()) return false;
	}

	ui_camera = GETH_COMP(ui_camera_h, TCompCamera);
	if (!ui_camera) return false;

	return true;
}

void TCompGuiCursor::update(float dt)
{
	if (!enabled) return;
	if (myGui->getCursor() != MY_OWNER) return;
	updateMovement(dt);
	updateNavigation();
}

void TCompGuiCursor::updateNavigation()
{
	if (init_pos) {
		init_pos = false;
		x = init_x;
		y = init_y;
		CHandle next_gui = TCompGui::getMatrixHandle(menu_name, x, y);
		if (next_gui.isValid()) putCursorOn(next_gui);
	}
	else {
		int * current = nullptr;
		int dir = 0;
		int max = 0;
		if (controller->IsUpPressed()) {
			current = &y;
			dir = -1;
			max = GUI_MAX_ROW;
		}
		else if (controller->IsDownPressed()) {
			current = &y;
			dir = 1;
			max = GUI_MAX_ROW;
		}
		else if (controller->IsLeftPressed()) {
			current = &x;
			dir = -1;
			max = GUI_MAX_COL;
		}
		else if (controller->IsRightPressed()) {
			current = &x;
			dir = 1;
			max = GUI_MAX_COL;
		}
		if (current) {
			int prev = *current;
			bool found = false;
			auto gui_matrix = TCompGui::getGuiMatrix(menu_name);
			do {
				*current += dir;
				*current = (max + *current) % max;
				CHandle next_gui = gui_matrix.elem[x][y];
				if (next_gui.isValid()) {
					putCursorOn(next_gui);
					found = true;
				}
			} while (!found && *current != prev);
		}
	}
}

void TCompGuiCursor::putCursorOn(CHandle next_gui)
{
	assert(next_gui.isValid());
	GET_COMP(tmx, next_gui, TCompTransform);
	if (tmx) myTransform->setPosition(tmx->getPosition());
	GET_COMP(gui, next_gui, TCompGui);
	VEC3 offset = VEC3(gui->GetWidth(), -gui->GetHeight(), 0.f);
	myTransform->addPosition(offset*0.4f);
}

void TCompGuiCursor::updateMovement(float dt)
{
	//Leer mouse
	float dx = 0, dy = 0;
	dx += controller->MouseDeltaX();
	dy += controller->MouseDeltaY();

	//Apply speed
	///float smooth = 0.99f;
	///factor = (speed * dt * (1 - smooth)) + factor * smooth;
	dx *= speed; //* dt;
	dy *= speed; //* dt;

	//Calc Movement
	VEC3 movement = VEC3(dx, -dy, 0);
	VEC3 new_pos = myTransform->getPosition() + movement;
	///VEC3 new_pos_old = new_pos;

	//Limits camera
	VEC3 min_ortho = ui_camera->getMinOrtho();
	VEC3 max_ortho = ui_camera->getMaxOrtho();
	clampVector(new_pos, ui_camera->getMinOrtho(), ui_camera->getMaxOrtho());

	//Apply movement
	myTransform->setPosition(new_pos);
}

void TCompGuiCursor::renderInMenu()
{
	ImGui::Text("Is overbutton %d", button.isValid());
	if (menu_name != "") {
		auto screen = TCompGui::getGuiMatrix(menu_name);//gui_screens[menu_name];
		for (int i = 0; i < GUI_MAX_ROW; i++) {
			for (int j = 0; j < GUI_MAX_ROW; j++) {
				if (screen.elem[i][j].isValid())
					ImGui::Text("screen(%d,%d): %s", i, j, GET_NAME(screen.elem[i][j]));
			}
		}
	}
}

void TCompGuiCursor::onButton(const TMsgOverButton& msg)
{
	if (msg.is_over) {
		button = msg.button;
		GET_COMP(gui_basic, button, TCompGui);
		if (gui_basic) {
			x = gui_basic->GetCol();
			y = gui_basic->GetRow();
		}
	}
	else {
		if (button == msg.button) {
			button = CHandle();
		}
	}
}

TCompGuiCursor::~TCompGuiCursor()
{
	if (menu_name != "") TCompGui::clearScreen(menu_name);
	//GameController->SetGameState(CGameController::RUNNING);
}