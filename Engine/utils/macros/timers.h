#ifndef INC_MACROS_TIMER_H_
#define INC_MACROS_TIMER_H_

#define ____TIMER Timers. Declara par de floats que representan un timer, comprobar si llegan a cero, ...
#define ____TIMER_DECLARE_VALUE_(name, value)	float name = value, _##name = value;
#define ____TIMER_DECLARE_(name)				float name, _##name;
#define ____TIMER_REDEFINE_(name, value)		name = value; _##name = value
#define ____TIMER_RESET_(name)					name = _##name

#define ____TIMER_CHECK_DO_(name)				if (____TIMER__END_(name)) {
#define ____TIMER_CHECK_DONE_(name)				____TIMER_RESET_(name); } else {	____TIMER__UPDATE_(name); }
#define ____TIMER__END_(name)					(name < 0)
#define ____TIMER__SET_ZERO_(name)				name = -1
//____TIMER_CHECK_DO_ and ____TIMER_CHECK_DONE_
//If timer ends, doSometing and reset timer
//else update timer
//Use:
//____TIMER_CHECK_DO_(timerName)
//doSometing
//____TIMER_CHECK_DONE_(timerName)
//

// Auxiliars
#define ____TIMER__IF_END_(name)		if (____TIMER__END_(name))
#define ____TIMER__UPDATE_(name)		if (name > -1) name -= getDeltaTime()

#endif
