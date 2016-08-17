#ifndef INC_MODULE_GUI_BUTTON_H_
#define INC_MODULE_GUI_BUTTON_H_

#include "components/comp_base.h"

//Forward Declaration
class TCompTransform;

class TCompGuiButton : public TCompBase {
	float width;
	float height;
	bool cursor_over = false;

	TCompTransform * myTransform;
	TCompTransform * cursorTransform;
public:
	// load Xml
	bool load(MKeyValue& atts);

	//void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo();
	void update(float dt);
	void renderInMenu();
};

#endif
