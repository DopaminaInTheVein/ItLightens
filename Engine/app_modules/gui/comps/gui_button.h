#ifndef INC_MODULE_GUI_BUTTON_H_
#define INC_MODULE_GUI_BUTTON_H_

#include "components/comp_base.h"
#include "logic/aicontroller.h"

#include "gui_basic.h"

//Forward Declaration
class TCompTransform;
class TCompGui;

class TCompGuiButton : public aicontroller, public TCompBase {
	bool init_enabled;
	bool language;

	//State info
	CHandle cursor;
	GuiListener listener;
	float render_state;
	float render_state_target;
	//float render_state_speed;

	TCompTransform * myTransform;
	TCompTransform * cursorTransform;
	TCompGui * myGui;

	// Aux
	bool checkOver();
	bool checkClicked();
	bool checkReleased();
	void notifyOver(bool);

	enum RStates {
		DISABLED = 0,
		ENABLED,
		OVER,
		CLICKED,
		RELEASED,
		SIZE
	};

	//Static info
	static void loadOptions();
	static bool options_loaded;
	static float * speeds_increase;
	static float * speeds_decrease;
	static float t_enabled, t_disabled, t_over, t_unover, t_clicked, t_unclicked, t_released, t_unreleased;

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;
	map<string, statehandler>* getStatemap();

public:

	// load Xml
	bool load(MKeyValue& atts);

	void onCreate(const TMsgEntityCreated&);
	bool getUpdateInfo();
	void update(float dt);
	void renderInMenu();

	//States
	void AddButtonStates();
	void Disabled();
	void Enabled();
	void Over();
	void Clicked();
	void Released();
	void Actioned();

	//Messages
	void onClick(const TMsgClicked&);
	void onSetListener(const TMsgGuiSetListener&);

	//Render State
	void updateRenderState();
	void updateSize();
	float getRenderState() { return render_state; }

	//Aux
	void setReleased();
	void execute();
};

#endif
