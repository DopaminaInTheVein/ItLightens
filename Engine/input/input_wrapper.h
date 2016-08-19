#ifndef _INC_INPUT_WRAPPER_H_
#define _INC_INPUT_WRAPPER_H_

#include "input.h"

class CInputWrapper
{
public:
	// Left Joystick
	bool IsMoveForward();
	bool IsMoveRight();
	bool IsMoveBackWard();
	bool IsMoveLeft();

	// Right Joystick
	bool IsCamMovedUp(float right_stick_sensibility);
	bool IsCamMovedDown(float right_stick_sensibility);
	bool IsCamMovedLeft(float right_stick_sensibility);
	bool IsCamMovedRight(float right_stick_sensibility);

	float JoystickDeltaRightX();
	float JoystickDeltaRightY();
	float JoystickRightX();
	float JoystickRightY();
	bool IsJoystickRXMax();
	bool IsJoystickRXMin();

	//Joystick min & max
	float JoystickMax();
	float JoystickMin();

	// Mouse
	float MouseDeltaX();
	float MouseDeltaY();

	// A-B-X-Y
	bool IsActionButtonPessed();
	bool IsJumpButtonPressed();
	bool IsPossessionButtonPressed();
	bool IsSenseButtonPressed();

	// (LB - RB)
	bool IsPlusPolarityPressed();
	bool IsMinusPolarityPressed();
	bool IsCameraUpPressing();
	bool IsCameraDownPressing();

	// Start & back
	bool IsPausePressed();
	bool IsBackPressed();

	// Capture & Release Mouse
	void ChangeMouseState(bool captured);

	// DEBUG TOOLS
	bool isCameraReleaseButtonPressed();
	bool isReleaseButtonPressed();
	bool isTeleportComboButtonPressed();
	bool isRenderDebugComboButtonPressed();

	bool isToogleCommandLogButtonPressed();
	bool isToogleConsoleLoguttonPressed();
	bool isPauseGameButtonPressed();
	bool isStopGameButtonPressed();

	bool isSlowButtonPressed();
	bool isDrawLineButtonPressed();
	bool isTestSSAOButoonPressed();
	bool isEspecialVisionButtonPressed();
	bool isEditorLightsButtonPressed();
	bool isParticleEditorButtonPressed();
	bool isParticleEditorActivationPressed();
	bool interruptGuardShotButtonPressed();

	bool changegui();
};

extern CInputWrapper * controller;
#endif
