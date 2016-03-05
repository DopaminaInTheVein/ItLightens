#include "mcv_platform.h"
#include "keyboard.h"

void TKeyBoard::sysSysStatus(int key_code, bool is_pressed) {
  active_keys[key_code].setCurrentStatus(is_pressed);
}

void TKeyBoard::update(float dt) {

  sysSysStatus(VK_CONTROL, isKeyPressed(VK_CONTROL));
  sysSysStatus(VK_LWIN, isKeyPressed(VK_LWIN));
  sysSysStatus(VK_SHIFT, isKeyPressed(VK_SHIFT));
  sysSysStatus(VK_MENU, isKeyPressed(VK_MENU)); //ALT
  sysSysStatus(VK_SPACE, isKeyPressed(VK_SPACE));
  sysSysStatus('W', isKeyPressed('W'));
  sysSysStatus('A', isKeyPressed('A'));
  sysSysStatus('S', isKeyPressed('S'));
  sysSysStatus('D', isKeyPressed('D'));
  sysSysStatus('T', isKeyPressed('T'));

  for (auto& it : active_keys) {
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