#include "mcv_platform.h"
#include "gui_menu_pause.h"
#include "gui_button_std.h"
#include "imgui/imgui_internal.h"

CGuiMenuPause::CGuiMenuPause()
{
	// Config
	// ----------------------------
	RectNormalized rn = RectNormalized(0.3f, 0.1f, 0.4f, 0.8f);
	rect = Rect(rn);
	float sizeXButtons = 0.2f;
	float sizeYButtons = 0.1f;
	float spaceYButtons = 0.05f;
	float startOffsetY = 0.3f;
	float fontSize = sizeYButtons - 0.02f;
	// ----------------------------

	// Calc
	// ----------------------------
	float startYButtons = rn.y + 0.2f;
	float startXButtons = (rn.x * 2 + rn.sx - sizeXButtons) / 2.f;
	float distanceYButtons = spaceYButtons + sizeYButtons;
	// ----------------------------


	btnResume = new CGuiButtonStd(
		GUI::createRect(startXButtons, startYButtons, sizeXButtons, sizeYButtons),
		fontSize,
		"Resume"
	);

	btnExit = new CGuiButtonStd(
		GUI::createRect(startXButtons, startYButtons + distanceYButtons*1, sizeXButtons, sizeYButtons),
		fontSize,
		"Exit"
	);
}


void CGuiMenuPause::render()
{
	// Menu Frame
	GUI::drawRect(rect, GUI::IM_BLACK_TRANSP);

	// Menu Text
	GUI::drawText(0.45f, 0.1f, GImGui->Font, 0.1f, GUI::IM_WHITE, "MENU");

	// Buttons
	if (btnResume->render()) {
		io->mouse.capture();
		GameController->SetGameState(CGameController::RUNNING);
	}
	
	if (btnExit->render()) {
		CApp::get().exitGame();
	}
}

