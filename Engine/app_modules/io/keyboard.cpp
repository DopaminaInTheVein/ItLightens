#include "mcv_platform.h"
#include "keyboard.h"

void TKeyBoard::sysSysStatus(int key_code, const bool& is_pressed) {
  active_keys[key_code].setCurrentStatus(is_pressed);
}

void TKeyBoard::update(float dt) {

  for (auto& it : active_keys) {
	  //if (!it.second.isPressed()) {
		  it.second.setCurrentStatus(it.second.isPressed());
		  it.second.update(dt);
  }
}

const CDigitalButton &TKeyBoard::operator[](int key_code) {
  static CDigitalButton non_pressed_key;
  auto it = active_keys.find(key_code);
  if (it != active_keys.end())
    return it->second;
  return non_pressed_key;
}