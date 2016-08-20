#include "mcv_platform.h"
#include <windows.h>
#include "input_wrapper.h"
#include "input.h"
#include "app_modules\io\io.h"

extern CInputWrapper* controller = new CInputWrapper;

bool CInputWrapper::IsCamMovedUp(float right_stick_sensibility) {
	return false;
}
bool CInputWrapper::IsCamMovedDown(float right_stick_sensibility) {
	return false;
}
bool CInputWrapper::IsCamMovedLeft(float right_stick_sensibility) {
	return false;
}
bool CInputWrapper::IsCamMovedRight(float right_stick_sensibility) {
	return false;
}

// Right Joystick
bool CInputWrapper::IsMoveForward() {
	return io->keys['W'].isPressed() || io->joystick.ly > 0;
}
bool CInputWrapper::IsMoveRight() {
	return io->keys['D'].isPressed() || io->joystick.lx > 0;
}
bool CInputWrapper::IsMoveBackWard() {
	return io->keys['S'].isPressed() || io->joystick.ly < 0;
}
bool CInputWrapper::IsMoveLeft() {
	return io->keys['A'].isPressed() || io->joystick.lx < 0;
}
float CInputWrapper::JoystickDeltaRightX() {
	return io->joystick.drx;
}
float CInputWrapper::JoystickDeltaRightY() {
	return io->joystick.dry;
}
float CInputWrapper::JoystickRightX() {
	return io->joystick.rx;
}
float CInputWrapper::JoystickRightY() {
	return io->joystick.ry;
}
bool CInputWrapper::IsJoystickRXMax() {
	return io->joystick.rx == io->joystick.max_stick_value;
}
bool CInputWrapper::IsJoystickRXMin() {
	return io->joystick.rx == io->joystick.min_stick_value;
}

//Joystick min & max

float CInputWrapper::JoystickMax() {
	return io->joystick.max_stick_value;
}
float CInputWrapper::JoystickMin() {
	return io->joystick.min_stick_value;
}

// Mouse
float CInputWrapper::MouseDeltaX() {
	return io->mouse.dx;
}
float CInputWrapper::MouseDeltaY() {
	return io->mouse.dy;
}
// A-B-X-Y
bool CInputWrapper::IsActionButtonPessed() {
	return io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed();
}
bool CInputWrapper::IsJumpButtonPressed() {
	return io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed();
}
bool CInputWrapper::IsPossessionButtonPressed() {
	return io->keys[VK_SHIFT].becomesPressed() || io->joystick.button_Y.becomesPressed();
}
bool CInputWrapper::IsSenseButtonPressed() {
	return io->mouse.right.becomesPressed() || io->joystick.button_B.becomesPressed();
}

// (LB - RB)
bool CInputWrapper::IsPlusPolarityPressed() {
	return io->keys['Q'].becomesPressed() || io->joystick.button_L.becomesPressed();
}
bool CInputWrapper::IsMinusPolarityPressed() {
	return io->keys['E'].becomesPressed() || io->joystick.button_R.becomesPressed();
}
bool CInputWrapper::IsCameraUpPressing() {
	return io->keys['Q'].isPressed() || io->joystick.button_L.isPressed();
}
bool CInputWrapper::IsCameraDownPressing() {
	return io->keys['E'].isPressed() || io->joystick.button_R.isPressed();
}

// Start & back
bool CInputWrapper::IsPausePressed() {
	return io->keys[VK_RETURN].becomesPressed() || io->joystick.button_START.becomesPressed();
}
bool CInputWrapper::IsBackPressed() {
	return io->keys[VK_ESCAPE].becomesPressed() || io->joystick.button_BACK.becomesPressed();
}

// Capture & Release Mouse
void CInputWrapper::ChangeMouseState(bool captured) {
	if (captured) {
		io->mouse.capture();
	}
	else {
		io->mouse.release();
	}
}

// DEBUG TOOLS
#ifndef FINAL_BUILD

bool CInputWrapper::isCameraReleaseButtonPressed() {
	return io->keys['K'].becomesPressed();
}
bool CInputWrapper::isTeleportComboButtonPressed() {
	return io->keys['T'].becomesPressed() && io->keys[VK_CONTROL].isPressed();
}
bool CInputWrapper::isRenderDebugComboButtonPressed() {
	return io->keys['N'].becomesPressed() && io->keys[VK_CONTROL].isPressed();
}
bool CInputWrapper::isReleaseButtonPressed() {
	return io->keys[220].becomesPressed();
}
bool CInputWrapper::isToogleCommandLogButtonPressed() {
	return io->keys['O'].becomesPressed();
}
bool CInputWrapper::isToogleConsoleLoguttonPressed() {
	return io->keys['L'].becomesPressed();
}
bool CInputWrapper::isPauseGameButtonPressed() {
	return io->keys['I'].becomesPressed();
}
bool CInputWrapper::isStopGameButtonPressed() {
	return io->keys['P'].becomesPressed();
}
bool CInputWrapper::isSlowButtonPressed() {
	return io->keys['M'].becomesPressed();
}
bool CInputWrapper::isDrawLineButtonPressed() {
	return io->keys['N'].becomesPressed();
}
bool CInputWrapper::isTestSSAOButoonPressed() {
	return io->keys[VK_F3].becomesPressed();
}
bool CInputWrapper::isEspecialVisionButtonPressed() {
	return io->keys['H'].becomesPressed();
}
bool CInputWrapper::isEditorLightsButtonPressed() {
	return io->keys[VK_F9].becomesPressed();
}
bool CInputWrapper::isParticleEditorActivationPressed() {
	return io->keys[VK_F8].becomesPressed();
}
bool CInputWrapper::interruptGuardShotButtonPressed() {
	return io->keys['B'].becomesPressed();
}
bool CInputWrapper::changegui() {
	return io->keys[VK_F1].becomesPressed();
}
#endif