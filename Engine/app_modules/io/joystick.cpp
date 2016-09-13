#include "mcv_platform.h"
#include "joystick.h"
#include "io.h"

float TJoystick::t_before_repeat = 0.5f;
float TJoystick::t_repeat = 0.25f;

TJoystick::TJoystick()
{
	// Set the Controller Number
	_controllerNum = 0;
}

TJoystick::TJoystick(int playerNumber = 1)
{
	// Set the Controller Number
	_controllerNum = playerNumber - 1;
}

XINPUT_STATE TJoystick::GetState()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	XInputGetState(_controllerNum, &_controllerState);

	return _controllerState;
}

bool TJoystick::IsConnected()
{
	// Zeroise the state
	ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));

	// Get the state
	DWORD Result = XInputGetState(_controllerNum, &_controllerState);

	if (Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void TJoystick::Vibrate(int leftVal, int rightVal)
{
	// Create a Vibraton State
	XINPUT_VIBRATION Vibration;

	// Zeroise the Vibration
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	// Set the Vibration Values
	Vibration.wLeftMotorSpeed = leftVal;
	Vibration.wRightMotorSpeed = rightVal;

	// Vibrate the controller
	XInputSetState(_controllerNum, &Vibration);
}

void TJoystick::update(float dt) {
	if (IsConnected()) {
		old_rx = rx;
		old_ry = ry;

		// Left and right stick axis values
		lx = GetState().Gamepad.sThumbLX;
		ly = GetState().Gamepad.sThumbLY;
		rx = GetState().Gamepad.sThumbRX;
		ry = GetState().Gamepad.sThumbRY;
		// Right stick delta values
		drx = (rx - old_rx) / right_stick_x_sensibility;
		dry = (ry - old_ry) / right_stick_y_sensibility;
		// Button state
		button_A.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
		button_A.update(dt);
		button_B.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
		button_B.update(dt);
		button_X.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
		button_X.update(dt);
		button_Y.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
		button_Y.update(dt);
		button_R.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);
		button_R.update(dt);
		button_L.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
		button_L.update(dt);
		button_START.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
		button_START.update(dt);
		button_BACK.setCurrentStatus((GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);
		button_BACK.update(dt);
		// Trigger values
		button_LT = GetState().Gamepad.bLeftTrigger;
		button_RT = GetState().Gamepad.bRightTrigger;

		//Stick pulsation
		lstick.update(getLX(), getLY(), dt);
		rstick.update(getRX(), getRY(), dt);
	}
}

int TJoystick::getLX()
{
	return abs(lx) > joystick_umbral ? lx : 0;
}
int TJoystick::getLY()
{
	return abs(ly) > joystick_umbral ? ly : 0;
}
int TJoystick::getRX()
{
	return abs(rx) > joystick_umbral ? rx : 0;
}
int TJoystick::getRY()
{
	return abs(ry) > joystick_umbral ? ry : 0;
}

void TJoystick::stick::update(int dx, int dy, float dt)
{
	int new_dir = 5; // direction as numpad
	new_dir += dx > 0.f ? 1 : dx < 0.f ? -1 : 0;
	new_dir += dy > 0.f ? 3 : dy < 0.f ? -3 : 0;

	if (dir != new_dir) {
		first_pressed = false;
		becomes_pressed = true;
	}
	else {
		time += dt;
		if (first_pressed) {
			becomes_pressed = time > t_repeat;
		}
		else {
			becomes_pressed = first_pressed = time > t_before_repeat;
		}
	}
	if (becomes_pressed) time = 0.f;
	dir = new_dir;
}
bool TJoystick::stick::LeftPressed()
{
	return dir % 3 == 1;
}
bool TJoystick::stick::RightPressed()
{
	return dir % 3 == 0;
}
bool TJoystick::stick::UpPressed()
{
	return dir > 6;
}
bool TJoystick::stick::DownPressed()
{
	return dir < 4;
}
#define StickBecomesPressed(dir) bool TJoystick::stick::##dir##BecomesPressed() { return becomes_pressed && ##dir##Pressed(); }
StickBecomesPressed(Left);
StickBecomesPressed(Right);
StickBecomesPressed(Up);
StickBecomesPressed(Down);

void TJoystick::start() {}