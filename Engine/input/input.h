#ifndef _INC_INPUT_H_
#define _INC_INPUT_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class CInput
{
public:
	CInput();
	CInput(const CInput&);
	~CInput();

	const int mouse_LEFT = 0;
	const int mouse_RIGHT = 1;
	const int mouse_WHEEL = 2;
	const int mouse_EXTRA = 3;

	const long joystick_axis_max = 65535;
	const long joystick_axis_min = 0;
	const long joystick_sensibility = 10000;

	const int joystick_A = 0;
	const int joystick_B = 1;
	const int joystick_X = 2;
	const int joystick_Y = 3;
	const int joystick_LB = 4;
	const int joystick_RB = 5;
	const int joystick_BACK = 6;
	const int joystick_START = 7;

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	void GetMouseLocation(int&, int&);
	bool IsLeftClickPressed();
	bool IsRightClickPressed();

	bool IsEscapePressed();
	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsUpPressed();
	bool IsOrientLeftPressed();
	bool IsOrientRightPressed();

	bool IsDownPressed();
	bool IsSpacePressed();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	bool ReadJoystick();
	void ProcessInput();
	void getMouseButtonPressed(int &pressed);
	bool getJoystickButtonPressed(int &pressed);

private:

	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;
	IDirectInputDevice8* m_joystick;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;
	DIJOYSTATE m_joystickState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};

#endif
