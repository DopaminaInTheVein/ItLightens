#ifndef INC_IO_DIGITAL_BUTTON_H_
#define INC_IO_DIGITAL_BUTTON_H_

#define TIME_FIRST_REPEAT 0.5f
#define TIME_REPEAT 0.25f

class CDigitalButton {
	bool pressed;
	bool prev_pressed;
	bool repeating;
	bool pressed_by_repeat;
	float time_pressed;
	float time_repeated;
	int times_called;
public:
	//static float TIME_FIRST_REPEAT;
	//static float TIME_REPEAT;

	CDigitalButton()
		: pressed(false)
		, pressed_by_repeat(false)
		, prev_pressed(false)
		, time_pressed(0.f)
		, times_called(0) {}

	void setCurrentStatus(bool is_pressed_now) {
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
	void update(float dt) {
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
	bool isPressed() const { return pressed; }
	bool isReleased() const { return !pressed; }
	bool becomesPressed() const { return pressed && !prev_pressed; }
	bool becomesPressedWithRepeat() const { return pressed && pressed_by_repeat; }
	bool becomesReleased() const { return !pressed && prev_pressed; }
	float timePressed() const { return time_pressed; }

	//TODO: REMOVE!!!!!!!
	bool getPressed() const { return pressed; }
	bool getPrevP() const { return prev_pressed; }
	int getTimes() const { return times_called; }
};
#endif
