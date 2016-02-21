#include "mcv_platform.h"
#include <windows.h>
#include "input.h"

CInput::CInput()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
	m_joystick = 0;
}


CInput::CInput(const CInput& other)
{
}


CInput::~CInput()
{
}


bool CInput::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = 0;
	m_mouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	/** KEYBOARD **/

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	/** MOUSE **/

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	/** JOYSTICK **/

	// Initialize the direct input interface for the joystick.
	result = m_directInput->CreateDevice(GUID_Joystick, &m_joystick, NULL);
	if (FAILED(result))
	{
		return DIENUM_CONTINUE;
	}

	// Make sure we got a joystick
	if (m_joystick == NULL) {
		printf("Joystick not found.\n");
		return false;
	}

	// Set the data format for the joystick using the pre-defined joystick data format.
	result = m_joystick->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the joystick to share with other programs.
	result = m_joystick->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Acquire the joystick.
	result = m_joystick->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void CInput::Shutdown()
{
	// Release the mouse.
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the joystick.
	if (m_joystick)
	{
		m_joystick->Unacquire();
		m_joystick->Release();
		m_joystick = 0;
	}

	// Release the main interface to direct input.
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool CInput::Frame()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// Read the current state of the joystick.
	result = ReadJoystick();
	if (!result)
	{
		return false;
	}

	// Process the changes in the mouse, keyboard and joystick.
	ProcessInput();

	return true;
}

bool CInput::ReadKeyboard()
{
	HRESULT result;

	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool CInput::ReadMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool CInput::ReadJoystick()
{
	HRESULT result;

	// If there's no joystick available, we must continue
	if (m_joystick == nullptr) {
		return false;
	}

	// Read the joystick device.
	result = m_joystick->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&m_joystickState);
	if (FAILED(result))
	{
		// If the joystick lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_joystick->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

/*** MOUSE ***/

void CInput::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > m_screenWidth) { m_mouseX = m_screenWidth; }
	if (m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }

	return;
}

// Gets mouse location
void CInput::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}

// Detects whether the left mouse button is pressed
bool CInput::IsLeftClickPressed() {

	int mouseButtonPressed = 10;
	getMouseButtonPressed(mouseButtonPressed);

	int buttonPressed = 50;
	bool joystickButtonPressed = getJoystickButtonPressed(buttonPressed);

	if ((mouseButtonPressed == mouse_LEFT) || (joystickButtonPressed && buttonPressed == joystick_X))
	{
		return true;
	}

	return false;
}

// Detects wheter the right mouse is pressed
bool CInput::IsRightClickPressed() {

	int mouseButtonPressed = 10;
	getMouseButtonPressed(mouseButtonPressed);

	int buttonPressed = 50;
	bool joystickButtonPressed = getJoystickButtonPressed(buttonPressed);

	if ((mouseButtonPressed == mouse_RIGHT) || (joystickButtonPressed && buttonPressed == joystick_B))
	{
		return true;
	}

	return false;
}

// Detects which mouse button has been pressed
void CInput::getMouseButtonPressed(int &pressed) {

	for (int i = 0; i < 4; i++) {
		if (m_mouseState.rgbButtons[i] != 0)
			pressed = i;
	}

}

/*** KEYBOARD ***/

// Detects if the space is pressed
bool CInput::IsEscapePressed()
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

// Detects if the Left direction is pressed (key A)
bool CInput::IsLeftPressed() {
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((m_keyboardState[DIK_A] & 0x80) || (m_joystick != nullptr && m_joystickState.lX < joystick_sensibility - joystick_axis_min))
	{
		return true;
	}

	return false;
}

// Detects if the right direction is pressed (key D)
bool CInput::IsRightPressed() {
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((m_keyboardState[DIK_D] & 0x80) || (m_joystick != nullptr && m_joystickState.lX > (joystick_axis_max - joystick_sensibility)))
	{
		return true;
	}

	return false;
}
// Detects if the Left orientation is pressed (key Q)
bool CInput::IsOrientLeftPressed() {
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((m_keyboardState[DIK_Q] & 0x80) || (m_mouseState.lZ < 0) || (m_joystick != nullptr && m_joystickState.lRx < joystick_sensibility - joystick_axis_min))
	{
		return true;
	}

	return false;
}

// Detects if the right orientation is pressed (key E)
bool CInput::IsOrientRightPressed() {
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((m_keyboardState[DIK_E] & 0x80) || (m_mouseState.lZ > 0) || (m_joystick != nullptr && m_joystickState.lRx > (joystick_axis_max - joystick_sensibility)))
	{
		return true;
	}

	return false;
}

// Detects if the up direction is pressed (key W)
bool CInput::IsUpPressed() {
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((m_keyboardState[DIK_W] & 0x80) || (m_joystick != nullptr && m_joystickState.lY < (joystick_sensibility - joystick_axis_min)))
	{
		return true;
	}

	return false;
}

// Detects if the down direction is pressed (key S)
bool CInput::IsDownPressed() {
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((m_keyboardState[DIK_S] & 0x80) || (m_joystick != nullptr && m_joystickState.lY > (joystick_axis_max - joystick_sensibility)))
	{
		return true;
	}

	return false;
}

// Detects if the space key was pressed
bool CInput::IsSpacePressed() {

	int buttonPressed = 50;
	bool joystickButtonPressed = getJoystickButtonPressed(buttonPressed);

	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((m_keyboardState[DIK_SPACE] & 0x80) || (joystickButtonPressed && buttonPressed == joystick_A))
	{
		return true;
	}

	return false;
}

/*** JOYSTICK ***/
// Detects which button of the joystick was pressed
bool CInput::getJoystickButtonPressed(int &pressed) {

	if (m_joystick == nullptr)
		return false;

	for (int i = 0; i < 32; i++) {
		if (m_joystickState.rgbButtons[i] != 0)
			pressed = i;
	}

	return true;
}
