#ifndef INC_MODULE_GUI_SELECTOR_H_
#define INC_MODULE_GUI_SELECTOR_H_

#include "components/comp_base.h"
#include "logic/aicontroller.h"

//Forward Declaration
class TCompTransform;
class TCompGui;
class TCompText;

class TCompGuiSelector : public aicontroller, public TCompBase {
	//struct SelectorOption {
	//	std::string text;
	//};

	//State info
	CHandle cursor;
	CHandle arrow_left;
	CHandle arrow_right;
	VHandles options;
	int cur_option;
	bool is_over = false;
	VEC3 my_pos;
	//std::vector<SelectorOption> options;

	TCompTransform * myTransform;
	TCompTransform * cursorTransform;
	TCompGui * myGui;

	// Aux
	bool checkOver();
	void notifyOver(bool over);
	void AddArrows();
	void AddArrow(CHandle&, string prefab, string name_event, float offset_pos);
	TCompText* setTextVisible(int option, bool visible, bool instant = false);

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
	void AddSelectorStates();
	void Disabled();
	void Enabled();
	void Over();

	//Messages
	void onCreate(const TMsgEntityCreated&);
	void onGuiNotify(const TMsgGuiNotify&);
	void onLanguageChanged(const TMsgLanguageChanged&);

	//Public actions
	int AddOption(std::string);
	void SelectOption(int);
};

#endif
