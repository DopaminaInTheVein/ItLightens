#ifndef _INC_INPUT_WRAPPER_H_
#define _INC_INPUT_WRAPPER_H_

#include "input.h"

#ifndef FINAL_BUILD
#define DECL_INPUT_DEBUG(name) bool name();
#else
#define DECL_INPUT_DEBUG(name) bool name(){return false;}
#endif

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
	DECL_INPUT_DEBUG(isCameraReleaseButtonPressed);
	DECL_INPUT_DEBUG(isReleaseButtonPressed);
	DECL_INPUT_DEBUG(isTeleportComboButtonPressed);
	DECL_INPUT_DEBUG(isRenderDebugComboButtonPressing);

	DECL_INPUT_DEBUG(isToogleCommandLogButtonPressed);
	DECL_INPUT_DEBUG(isToogleConsoleLoguttonPressed);
	DECL_INPUT_DEBUG(isPauseGameButtonPressed);
	DECL_INPUT_DEBUG(isStopGameButtonPressed);

	DECL_INPUT_DEBUG(isSlowButtonPressed);
	DECL_INPUT_DEBUG(isDrawLineButtonPressing);
	DECL_INPUT_DEBUG(isTestSSAOButoonPressed);
	DECL_INPUT_DEBUG(isEspecialVisionButtonPressed);
	DECL_INPUT_DEBUG(isEditorLightsButtonPressed);
	DECL_INPUT_DEBUG(isParticleEditorButtonPressed);
	DECL_INPUT_DEBUG(isParticleEditorActivationPressed);
	DECL_INPUT_DEBUG(interruptGuardShotButtonPressed);

	DECL_INPUT_DEBUG(changegui);
};

extern CInputWrapper * controller;
#endif
