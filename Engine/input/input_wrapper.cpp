#include "mcv_platform.h"
#include <windows.h>
#include "input_wrapper.h"
#include "input.h"
#include "app_modules\io\io.h"

extern CInputWrapper* controller = new CInputWrapper;

bool CInputWrapper::IsCamMovedUp() {
	return io->joystick.getRY() > 0 || io->mouse.dy > 0;
}
bool CInputWrapper::IsCamMovedDown() {
	return io->joystick.getRY() < 0 || io->mouse.dy < 0;
}
bool CInputWrapper::IsCamMovedLeft() {
	return io->joystick.getRX() < 0 || io->mouse.dx < 0;
}
bool CInputWrapper::IsCamMovedRight() {
	return io->joystick.getRX() > 0 || io->mouse.dx > 0;
}

// Left Joystick
bool CInputWrapper::IsMoveForward() {
	return io->keys['W'].isPressed() || io->joystick.getLY() > 0;
}
bool CInputWrapper::IsMoveRight() {
	return io->keys['D'].isPressed() || io->joystick.getLX() > 0;
}
bool CInputWrapper::IsMoveBackWard() {
	return io->keys['S'].isPressed() || io->joystick.getLY() < 0;
}
bool CInputWrapper::IsMoveLeft() {
	return io->keys['A'].isPressed() || io->joystick.getLX() < 0;
}

// TODO: Preguntar por becomes pressed del joystick --> simular pulsaciones
// Dejar pulsado --> simular comportamiento teclado (una pulsacion, espera, pulsaciones repetidas cada X milisegundos)
bool CInputWrapper::IsUpPressed() {
	return io->keys['W'].becomesPressedWithRepeat() || io->keys[VK_UP].becomesPressedWithRepeat() || io->joystick.lstick.UpBecomesPressed();
}
bool CInputWrapper::IsDownPressed() {
	return io->keys['S'].becomesPressedWithRepeat() || io->keys[VK_DOWN].becomesPressedWithRepeat() || io->joystick.lstick.DownBecomesPressed();
}
bool CInputWrapper::IsLeftPressed() {
	return io->keys['A'].becomesPressedWithRepeat() || io->keys[VK_LEFT].becomesPressedWithRepeat() || io->joystick.lstick.LeftBecomesPressed();
}
bool CInputWrapper::IsLeftPressedSelector() {
	return IsLeftPressed() || io->mouse.left.becomesPressed();
}
bool CInputWrapper::IsRightPressed() {
	return io->keys['D'].becomesPressedWithRepeat() || io->keys[VK_RIGHT].becomesPressedWithRepeat() || io->joystick.lstick.RightBecomesPressed();
}
bool CInputWrapper::IsRightPressedSelector() {
	return IsRightPressed() || io->mouse.right.becomesPressed();
}
float CInputWrapper::MoveYNormalized() {
	if (io->keys['W'].isPressed()) {
		return 1.0f;
	}
	else if (io->keys['S'].isPressed()) {
		return -1.0f;
	}
	else if (io->joystick.getLY() != 0.f) {
		return io->joystick.ly / (float)io->joystick.max_stick_value;
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
	else if (io->joystick.getLX() != 0.f) {
		return (-io->joystick.lx / (float)io->joystick.max_stick_value);
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
	if (io->joystick.getRX() > 0) {
		return io->joystick.rx;
	}
	else if (io->joystick.getRX() < 0) {
		return io->joystick.rx;
	}
	return 0.0f;
}
float CInputWrapper::JoystickRightY() {
	return io->joystick.getRY();
}
bool CInputWrapper::IsJoystickRXMax() {
	return io->joystick.getRX() == io->joystick.max_stick_value;
}
bool CInputWrapper::IsJoystickRXMin() {
	return io->joystick.getRX() == io->joystick.min_stick_value;
}
float CInputWrapper::RYNormalized() {
	if (io->mouse.dy != 0) {
		return io->mouse.dy;
	}
	else if (io->joystick.getRY() != 0.f) {
		return io->joystick.getRY() * 8 / (float)io->joystick.max_stick_value;
	}
	return 0.0f;
}
float CInputWrapper::RXNormalized() {
	if (io->mouse.dx != 0) {
		return io->mouse.dx;
	}
	else if (io->joystick.getRX() != 0.f) {
		return io->joystick.rx * 8 / (float)io->joystick.max_stick_value;
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
bool CInputWrapper::IsActionButtonReleased() {
	return io->mouse.left.becomesReleased() || io->joystick.button_X.becomesReleased();
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
bool CInputWrapper::ActionGuiButtonBecomesPressed() {
	return io->mouse.left.becomesPressed() || io->joystick.button_X.becomesPressed()
		|| io->keys[VK_RETURN].becomesPressed() || io->keys[VK_SPACE].becomesPressed();
}
bool CInputWrapper::ActionGuiButtonBecomesReleased() {
	return io->mouse.left.becomesReleased() || io->joystick.button_X.becomesReleased()
		|| io->keys[VK_RETURN].becomesReleased() || io->keys[VK_SPACE].becomesReleased();
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
bool CInputWrapper::IsBackBeingPressed() {
	return io->keys[VK_ESCAPE].isPressed() || io->joystick.button_BACK.isPressed();
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