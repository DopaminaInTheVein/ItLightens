#include "mcv_platform.h"
#include "gui_cursor.h"

#include "components/entity.h"
#include "components/comp_transform.h"
#include "components/comp_camera.h"

#include "app_modules/gameController.h"
#include "app_modules/io/io.h"

// load Xml
bool TCompGuiCursor::load(MKeyValue& atts)
{
	GameController->SetUiControl(true);
	GameController->SetGameState(CGameController::STOPPED);

	speed = atts.getFloat("speed", .5f);
	return true;
}

//void TCompGuiCursor::onCreate(const TMsgEntityCreated&)
//{
//	GameController->SetUiControl(true);
//}

bool TCompGuiCursor::getUpdateInfo()
{
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
	updateMovement(dt);
	if (button.isValid()) {
		if (io->mouse.left.becomesReleased()) {
			button.sendMsg(TMsgClicked());
		}
	}
}

void TCompGuiCursor::updateMovement(float dt)
{
	//Leer mouse
	float dx = 0, dy = 0;
	dx += io->mouse.dx;
	dy += io->mouse.dy;

	//Apply speed
	float smooth = 0.99f;
	factor = (speed * dt * (1 - smooth)) + factor * smooth;
	dx *= factor;
	dy *= factor;

	//Calc Movement
	VEC3 movement = VEC3(dx, -dy, 0);
	VEC3 new_pos = myTransform->getPosition() + movement;
	//VEC3 new_pos_old = new_pos;

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
}

void TCompGuiCursor::onButton(const TMsgOverButton& msg)
{
	if (msg.is_over) button = msg.button;
	else {
		if (button == msg.button) button = CHandle();
	}
}

TCompGuiCursor::~TCompGuiCursor()
{
	GameController->SetUiControl(false);
	//GameController->SetGameState(CGameController::RUNNING);
}