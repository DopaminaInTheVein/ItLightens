#include "mcv_platform.h"
#include "joystick.h"
#include "io.h"

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

	}
}

void TJoystick::start() {
}
