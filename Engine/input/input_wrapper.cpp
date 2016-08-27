#include "mcv_platform.h"
#include <windows.h>
#include "input_wrapper.h"
#include "input.h"
#include "app_modules\io\io.h"

extern CInputWrapper* controller = new CInputWrapper;

bool CInputWrapper::IsCamMovedUp() {
	return io->joystick.ry > joystick_umbral || io->mouse.dy > 0;
}
bool CInputWrapper::IsCamMovedDown() {
	return io->joystick.ry < -joystick_umbral || io->mouse.dy < 0;
}
bool CInputWrapper::IsCamMovedLeft() {
	return io->joystick.rx < -joystick_umbral || io->mouse.dx < 0;
}
bool CInputWrapper::IsCamMovedRight() {
	return io->joystick.rx > joystick_umbral || io->mouse.dx > 0;
}

// Left Joystick
bool CInputWrapper::IsMoveForward() {
	return io->keys['W'].isPressed() || io->joystick.ly > joystick_umbral;
}
bool CInputWrapper::IsMoveRight() {
	return io->keys['D'].isPressed() || io->joystick.lx > joystick_umbral;
}
bool CInputWrapper::IsMoveBackWard() {
	return io->keys['S'].isPressed() || io->joystick.ly < -joystick_umbral;
}
bool CInputWrapper::IsMoveLeft() {
	return io->keys['A'].isPressed() || io->joystick.lx < -joystick_umbral;
}

float CInputWrapper::MoveYNormalized() {
	if (io->keys['W'].isPressed()) {
		return 1.0f;
	}
	else if (io->keys['S'].isPressed()) {
		return -1.0f;
	}
	else if (io->joystick.ly > 0 && io->joystick.ly > joystick_umbral || io->joystick.ly < 0 && io->joystick.ly < -joystick_umbral) {
		return io->joystick.ly / io->joystick.max_stick_value;
	}
	return 0.0f;
}
float CInputWrapper::MoveXNormalized() {
	if (io->keys['D'].isPressed()) {
		return -1.0f;
	}
	else if (io->keys['A'].isPressed()) {
		return 1.0f;
	}
	else if (io->joystick.lx > 0 && io->joystick.lx > joystick_umbral || io->joystick.lx < 0 && io->joystick.lx < -joystick_umbral) {
		return -io->joystick.lx / io->joystick.max_stick_value;
	}
	return 0.0f;
}

// Right Joystick
float CInputWrapper::JoystickDeltaRightX() {
	return io->joystick.drx;
}
float CInputWrapper::JoystickDeltaRightY() {
	return io->joystick.dry;
}

float CInputWrapper::JoystickRightX() {
	if (io->joystick.rx > 0 && io->joystick.rx > joystick_umbral) {
		return io->joystick.rx;
	}
	else if (io->joystick.rx < 0 && io->joystick.rx < -joystick_umbral) {
		return io->joystick.rx;
	}
	return 0.0f;
}
float CInputWrapper::JoystickRightY() {
	if (io->joystick.ry > 0 && io->joystick.ry > joystick_umbral) {
		return io->joystick.ry;
	}
	else if (io->joystick.ry < 0 && io->joystick.ry < -joystick_umbral) {
		return io->joystick.ry;
	}
	return 0.0f;
}
bool CInputWrapper::IsJoystickRXMax() {
	return io->joystick.rx == io->joystick.max_stick_value;
}
bool CInputWrapper::IsJoystickRXMin() {
	return io->joystick.rx == io->joystick.min_stick_value;
}
float CInputWrapper::RYNormalized() {
	if (io->mouse.dy != 0) {
		return io->mouse.dy;
	}
	else if (io->joystick.ry > 0 && io->joystick.ry > joystick_umbral || io->joystick.ry < 0 && io->joystick.ry < -joystick_umbral) {
		return io->joystick.ry * 8 / io->joystick.max_stick_value;
	}
	return 0.0f;
}
float CInputWrapper::RXNormalized() {
	if (io->mouse.dx != 0) {
		return io->mouse.dx;
	}
	else if (io->joystick.rx > 0 && io->joystick.rx > joystick_umbral || io->joystick.rx < 0 && io->joystick.rx < -joystick_umbral) {
		return io->joystick.rx * 8 / io->joystick.max_stick_value;
	}
	return 0.0f;
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
	return io->mouse.left.isPressed() || io->joystick.button_X.isPressed();
}
bool CInputWrapper::IsJumpButtonPressed() {
	return io->keys[VK_SPACE].isPressed() || io->joystick.button_A.isPressed();
}
bool CInputWrapper::IsPossessionButtonPressed() {
	return io->keys[VK_SHIFT].isPressed() || io->joystick.button_Y.isPressed();
}
bool CInputWrapper::IsSenseButtonPressed() {
	return io->mouse.right.isPressed() || io->joystick.button_B.isPressed();
}

bool CInputWrapper::ActionButtonBecomesPessed() {
	return io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed();
}
bool CInputWrapper::JumpButtonBecomesPressed() {
	return io->keys[VK_SPACE].becomesPressed() || io->joystick.button_A.becomesPressed();
}
bool CInputWrapper::IsImpulseUpButtonPressed() {
	return io->keys[VK_SPACE].isPressed() || io->joystick.button_A.isPressed();
}
bool CInputWrapper::PossessionButtonBecomesPressed() {
	return io->keys[VK_SHIFT].becomesPressed() || io->joystick.button_Y.becomesPressed();
}
bool CInputWrapper::SenseButtonBecomesPressed() {
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
bool CInputWrapper::isRenderDebugComboButtonPressing() {
	return io->keys['N'].isPressed() && io->keys[VK_CONTROL].isPressed();
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
bool CInputWrapper::isDrawLineButtonPressing() {
	return io->keys['N'].isPressed();
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