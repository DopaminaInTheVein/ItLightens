#ifndef INC_IO_JOYSTICK_H_
#define INC_IO_JOYSTICK_H_

#include <XInput.h>
#include "digital_button.h"

#pragma comment(lib, "XInput.lib")

class TJoystick {

private:

	XINPUT_STATE _controllerState;
	int _controllerNum;

public:

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
  const int max_stick_value = 32767;
  const int min_trigger_value = 0;
  const int max_trigger_value = 255;

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

};

#endif

