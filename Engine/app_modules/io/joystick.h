#ifndef INC_IO_JOYSTICK_H_
#define INC_IO_JOYSTICK_H_

#include "digital_button.h"

class TJoystick {
  HWND hWnd;

public:

  // Left stick values
  int lx;
  int ly;
  // Right stick values
  int rx;
  int ry;

  //
  bool captured_by_app;

  // Main buttons
  CDigitalButton button_A;
  CDigitalButton button_B;
  CDigitalButton button_X;
  CDigitalButton button_Y;
  CDigitalButton button_R;
  CDigitalButton button_L;
  // Triggers
  CDigitalButton button_RT;
  CDigitalButton button_LT;
  // Option butons
  CDigitalButton button_START;
  CDigitalButton button_BACK;

  TJoystick();
  void start(HWND hWnd);
  void update(float dt);

};


#endif

