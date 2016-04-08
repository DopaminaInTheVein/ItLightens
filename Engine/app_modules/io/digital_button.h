#ifndef INC_IO_DIGITAL_BUTTON_H_
#define INC_IO_DIGITAL_BUTTON_H_

class CDigitalButton {
  bool pressed;
  bool prev_pressed;
  float time_pressed;
public:

  CDigitalButton()
    : pressed(false)
    , prev_pressed(false)
    , time_pressed(0.f)
  {}

  void setCurrentStatus(bool is_pressed_now) {
    prev_pressed = pressed;
    pressed = is_pressed_now;
    if (becomesPressed())
      time_pressed = 0.f;
  }
  void update(float dt) {
    if (isPressed())
      time_pressed += dt;
  }
  bool isPressed() const { return pressed; }
  bool isReleased() const { return !pressed; }
  bool becomesPressed() const { 
    return pressed && !prev_pressed; 
  }
  bool becomesReleased() const { 
    return !pressed && prev_pressed; 
  }
  float timePressed() const { return time_pressed; }
};

#endif

