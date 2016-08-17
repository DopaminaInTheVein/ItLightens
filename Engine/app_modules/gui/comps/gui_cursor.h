#ifndef INC_MODULE_GUI_CURSOR_H_
#define INC_MODULE_GUI_CURSOR_H_

#include "components/comp_base.h"

//Forward Declaration
class TCompTransform;

class TCompGuiCursor : public TCompBase {
	float speed;
	TCompTransform * myTransform;
public:
	// load Xml
	bool load(MKeyValue& atts);

	//void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo();
	void update(float dt);
	void renderInMenu();
	~TCompGuiCursor();
};

#endif
