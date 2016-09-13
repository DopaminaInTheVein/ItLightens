#ifndef INC_IO_JOYSTICK_H_
#define INC_IO_JOYSTICK_H_

#include <XInput.h>
#include "digital_button.h"

#pragma comment(lib,  "XInput9_1_0.lib")

class TJoystick {
private:

	XINPUT_STATE _controllerState;
	int _controllerNum;
	static float t_before_repeat;
	static float t_repeat;

public:
	struct stick {
		int dir;
		bool first_pressed;
		float time;
		bool becomes_pressed;
		stick() : dir(5), first_pressed(false), time(0.f), becomes_pressed(false) {}
		void update(int dx, int dy, float dt);
		bool LeftPressed();
		bool RightPressed();
		bool UpPressed();
		bool DownPressed();
		bool LeftBecomesPressed();
		bool RightBecomesPressed();
		bool UpBecomesPressed();
		bool DownBecomesPressed();
	};

	//Sticks
	stick lstick;
	stick rstick;

	// Left stick values (-32768 - 32767)
	int lx = 0;
	int ly = 0;
	// Right stick values
	int rx = 0;
	int ry = 0;
	int old_rx = 0;
	int old_ry = 0;

	int drx = 0;
	int dry = 0;
	// Trigger values (0 - 255)
	int button_LT = 0;
	int button_RT = 0;

	// Constants (gamepad taken values)
	const int min_stick_value = -32768;
	const int max_stick_value = 32768;
	const int min_trigger_value = 0;
	const int max_trigger_value = 255;
	int joystick_umbral = 5000; // 32768 - 15.26% Aprox

	// Config values
	const int right_stick_x_sensibility = 150;
	const int right_stick_y_sensibility = 150;

	// Main buttons
	CDigitalButton button_A;
	CDigitalButton button_B;
	CDigitalButton button_X;
	CDigitalButton button_Y;
	CDigitalButton button_R;
	CDigitalButton button_L;
	// Option butons
	CDigitalButton button_START;
	CDigitalButton button_BACK;

	TJoystick();
	TJoystick(int playerNumber);
	XINPUT_STATE GetState();
	bool IsConnected();
	void Vibrate(int leftVal = 0, int rightVal = 0);

	void start();
	void update(float dt);

	int getLX();
	int getLY();
	int getRX();
	int getRY();

	bool LeftBecomesPressed();
	bool RightBecomesPressed();
	bool UpBecomesPressed();
	bool DownBecomesPressed();
};

#endif
