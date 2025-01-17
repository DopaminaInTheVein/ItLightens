#include "mcv_platform.h"
#include "keyboard.h"

void TKeyBoard::sysSysStatus(int key_code, const bool& is_pressed) {
	active_keys[key_code].setCurrentStatus(is_pressed);
}

void TKeyBoard::update(float dt) {
	/*sysSysStatus(VK_CONTROL, isKeyPressed(VK_CONTROL));
	sysSysStatus(VK_LWIN, isKeyPressed(VK_LWIN));
	sysSysStatus(VK_SHIFT, isKeyPressed(VK_SHIFT));

	sysSysStatus(VK_MENU, isKeyPressed(VK_MENU)); //ALT*/

	sysSysStatus(VK_LSHIFT, isKeyPressed(VK_LSHIFT));

	for (auto& it : active_keys) {
		if (it.first != VK_LSHIFT) {
			it.second.setCurrentStatus(it.second.isPressed());
			it.second.update(dt);
		}
	}
}
void TKeyBoard::release() {
	for (auto& it : active_keys) {
		it.second.setCurrentStatus(false);
	}
}

const CKeyboardButton &TKeyBoard::operator[](int key_code) {
	static CKeyboardButton non_pressed_key;
	auto it = active_keys.find(key_code);
	if (it != active_keys.end())
		return it->second;
	return non_pressed_key;
}