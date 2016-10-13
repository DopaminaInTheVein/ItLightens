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

	CDigitalButton()
		: pressed(false)
		, pressed_by_repeat(false)
		, prev_pressed(false)
		, time_pressed(0.f)
		, times_called(0) {}

	virtual void setCurrentStatus(bool is_pressed_now);
	void update(float dt);
	bool isPressed() const;
	bool isReleased() const;
	bool becomesPressed() const;
	bool becomesPressedWithRepeat() const;
	bool becomesReleased() const;
	float timePressed() const;
};

class CPadButton : public CDigitalButton {
public:
	void setCurrentStatus(bool is_pressed_now) override;
};

class CMouseButton : public CDigitalButton {
public:
	void setCurrentStatus(bool is_pressed_now) override;
};

class CKeyboardButton : public CDigitalButton {
public:
	void setCurrentStatus(bool is_pressed_now) override;
};
#endif
