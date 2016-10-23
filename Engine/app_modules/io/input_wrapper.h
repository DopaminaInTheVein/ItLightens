#ifndef _INC_INPUT_WRAPPER_H_
#define _INC_INPUT_WRAPPER_H_

#include "io.h"

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
	float MoveYNormalized();
	float MoveXNormalized();
	//Left joystick becomes pressed
	bool CInputWrapper::IsUpPressed();
	bool CInputWrapper::IsDownPressed();
	bool CInputWrapper::IsLeftPressed();
	bool CInputWrapper::IsRightPressed();
	bool CInputWrapper::IsLeftPressedSelector();
	bool CInputWrapper::IsRightPressedSelector();

	// Right Joystick
	bool IsCamMovedUp();
	bool IsCamMovedDown();
	bool IsCamMovedLeft();
	bool IsCamMovedRight();

	float JoystickDeltaRightX();
	float JoystickDeltaRightY();
	float JoystickRightX();
	float JoystickRightY();

	float RYNormalized();
	float RXNormalized();

	//Joystick min & max
	bool IsJoystickRXMax();
	bool IsJoystickRXMin();
	float JoystickMax();
	float JoystickMin();

	// Mouse
	float MouseDeltaX();
	float MouseDeltaY();

	// A-B-X-Y
	bool IsActionButtonPessed();
	bool IsActionButtonReleased();
	bool IsJumpButtonPressed();
	bool IsPossessionButtonPressed();
	bool IsSenseButtonPressed();

	bool ActionButtonBecomesPessed();
	bool ActionGuiButtonBecomesPressed();
	bool ActionGuiButtonBecomesReleased();
	bool IsGuiItemDragged();
	bool JumpButtonBecomesPressed();
	bool IsImpulseUpButtonPressed();
	bool PossessionButtonBecomesPressed();
	bool SenseButtonBecomesPressed();

	// (LB - RB)
	bool IsPlusPolarityPressed();
	bool IsMinusPolarityPressed();
	bool IsCameraUpPressing();
	bool IsCameraDownPressing();

	// Start & back
	bool IsPausePressed();
	bool IsBackPressed();
	bool IsBackBeingPressed();
	bool IsEscapePressed();
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
	DECL_INPUT_DEBUG(isPauseDebugPressed);

	DECL_INPUT_DEBUG(isSlowButtonPressed);
	DECL_INPUT_DEBUG(isDrawLineButtonPressed);
	DECL_INPUT_DEBUG(isTestSSAOButoonPressed);
	DECL_INPUT_DEBUG(isEditorLightsButtonPressed);
	DECL_INPUT_DEBUG(isEditorMessagesButtonPressed);
	DECL_INPUT_DEBUG(isParticleEditorButtonPressed);
	DECL_INPUT_DEBUG(isParticleEditorActivationPressed);
	DECL_INPUT_DEBUG(interruptGuardShotButtonPressed);

	DECL_INPUT_DEBUG(changegui);
};

extern CInputWrapper * controller;
#endif
