#include "mcv_platform.h"
#include "digital_button.h"
#include "io.h"

void CDigitalButton::setCurrentStatus(bool is_pressed_now) {
	times_called++;
	prev_pressed = pressed;
	pressed = is_pressed_now;
	if (!pressed) pressed_by_repeat = false;
	if (becomesPressed()) {
		pressed_by_repeat = true;
		time_pressed = 0.f;
		time_repeated = 0.f;
		repeating = false;
	}
}
void CDigitalButton::update(float dt) {
	if (isPressed()) {
		time_pressed += dt;
		if (repeating) {
			time_repeated += dt;
			pressed_by_repeat = time_repeated > TIME_REPEAT;
			if (pressed_by_repeat) {
				time_repeated = 0.f;
			}
		}
		else {
			repeating = pressed_by_repeat = time_pressed > TIME_FIRST_REPEAT;
		}
	}
}
bool CDigitalButton::isPressed() const { return pressed; }
bool CDigitalButton::isReleased() const { return !pressed; }
bool CDigitalButton::becomesPressed() const { return pressed && !prev_pressed; }
bool CDigitalButton::becomesPressedWithRepeat() const { return pressed && pressed_by_repeat; }
bool CDigitalButton::becomesReleased() const { return !pressed && prev_pressed; }
float CDigitalButton::timePressed() const { return time_pressed; }

void CPadButton::setCurrentStatus(bool is_pressed_now) {
	CDigitalButton::setCurrentStatus(is_pressed_now);
	if (is_pressed_now) io->SetGamePadMode(true);
}

void CMouseButton::setCurrentStatus(bool is_pressed_now) {
	CDigitalButton::setCurrentStatus(is_pressed_now);
	if (is_pressed_now) io->SetGamePadMode(false);
}

void CKeyboardButton::setCurrentStatus(bool is_pressed_now) {
	CDigitalButton::setCurrentStatus(is_pressed_now);
	if (is_pressed_now) io->SetGamePadMode(false);
}