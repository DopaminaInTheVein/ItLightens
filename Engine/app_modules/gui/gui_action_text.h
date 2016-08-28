#ifndef INC_MODULE_GUI_ACTION_TEXT_H_
#define INC_MODULE_GUI_ACTION_TEXT_H_

#include "gui_utils.h"

enum eAction {
	NONE = 0
	, RECHARGE
	, RECHARGE_DRONE
	, REPAIR_DRONE
	, OVERCHARGE
	, POSSESS
	, ACTIVATE
	, PUT
	, EXAMINATE
	, DESTROY
	, CREATE_MAGNETIC_BOMB
	, CREATING
	, TRAVEL_WIRE
	, DREAM
	, LEAVE
	, SIZE_ACTIONS
};

//Forward Declaration
class Rect;

class CGuiActionText {
	Rect rect;
	float sizeFont;
	eAction action;
public:
	CGuiActionText(Rect r);

	void setState(eAction action);
	void update(float dt);
	void render();
};

#endif
