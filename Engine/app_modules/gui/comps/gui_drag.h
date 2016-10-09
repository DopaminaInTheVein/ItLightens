#ifndef INC_MODULE_GUI_DRAG_H_
#define INC_MODULE_GUI_DRAG_H_

#include "components/comp_base.h"
#include "logic/aicontroller.h"

//Forward Declaration
class TCompTransform;
class TCompGui;

class TCompGuiDrag : public aicontroller, public TCompBase {
	//State info
	CHandle cursor;
	CHandle drag_item;
	float value;
	float last_value;
	bool is_over = false;
	VEC3 my_pos;
	float render_state;

	TCompTransform * myTransform;
	TCompGui * myGui;
	TCompTransform * cursorTransform;
	TCompTransform * dragTransform;
	TCompGui * dragGui;

	// Aux
	bool checkOver();
	void notifyOver(bool over);
	void AddDragItem();
	bool checkEnabled();
	void enable();
	void disable();
	void updateRenderState();
	void updateDrag();
	void notifyValue();

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;
	map<string, statehandler>* getStatemap();

public:
	// load Xml
	bool load(MKeyValue& atts);

	bool getUpdateInfo();
	void update(float dt);
	void renderInMenu();

	//States
	void AddDragStates();
	void Disabled();
	void Enabled();
	void Over();
	void Clicked();

	//Messages
	void onCreate(const TMsgEntityCreated&);

	//Public actions
	void SetValue(float);
};

#endif
