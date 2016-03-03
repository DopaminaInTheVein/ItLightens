#include "mcv_platform.h"
#include <windows.h>
#include "input_manager.h"

CInputManager::CInputManager()
{
}

CInputManager::CInputManager(const CInputManager& other)
{
}

CInputManager::~CInputManager()
{
}

bool CInputManager::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	window = hwnd;
	screen_width = screenWidth;
	screen_height = screenHeight;

	return input.Initialize(hinstance, hwnd, screenWidth, screenHeight);
}

bool CInputManager::update()
{
	return input.Frame();
}

void CInputManager::exitGame(int exit_code) {

	input.Shutdown();
	PostQuitMessage(exit_code);

}

void CInputManager::lockMouse() {

	//NEED TO DETECT DEVICE WIDTH AND HEIGHT AUTOMATICALLY
	SetCursorPos(screen_width / 2, screen_height / 2);
	ShowCursor(false);
	mouse_locked = true;

}

void CInputManager::unlockMouse() {

	ShowCursor(true);
	mouse_locked = false;

}

void CInputManager::hideMenu() {

	//TODO
	menu_hidden = true;

}

void CInputManager::unhideMenu() {

	//TODO
	menu_hidden = false;

}