#ifndef INC_MODULE_GUI_CURSOR_H_
#define INC_MODULE_GUI_CURSOR_H_

#include "components/comp_base.h"

//Forward Declaration
class TCompTransform;
class TCompCamera;
class TCompGui;

class TCompGuiCursor : public TCompBase {
	std::string menu_name;
	float speed;
	int x, y; // position gui matrix
	bool init_pos = true;
	int init_x, init_y;
	//float factor = 0.f;
	TCompGui * myGui;
	TCompTransform * myTransform;
	TCompCamera * ui_camera;
	CHandle button;
	CHandle ui_camera_h;
	bool enabled = true;
	void putCursorOn(CHandle nextGui);
public:
	// load Xml
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo();
	void update(float dt);
	void updateMovement(float dt);
	void updateNavigation();
	void renderInMenu();
	bool isEnabled() { return enabled; }
	void setEnabled(bool) { enabled = false; }

	//Messages
	void onButton(const TMsgOverButton&);

	//Destructor
	~TCompGuiCursor();
};

#endif
