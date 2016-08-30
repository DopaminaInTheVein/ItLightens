#ifndef INC_MODULE_GUI_CURSOR_H_
#define INC_MODULE_GUI_CURSOR_H_

#include "components/comp_base.h"

//Forward Declaration
class TCompTransform;
class TCompCamera;

class TCompGuiCursor : public TCompBase {
	float speed;
	float factor = 0.f;
	TCompTransform * myTransform;
	TCompCamera * ui_camera;
	CHandle button;
	CHandle ui_camera_h;
public:
	// load Xml
	bool load(MKeyValue& atts);

	//void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo();
	void update(float dt);
	void updateMovement(float dt);
	void renderInMenu();

	//Messages
	void onButton(const TMsgOverButton&);

	//Destructor
	~TCompGuiCursor();
};

#endif
