#ifndef INC_MODULE_GUI_BASIC_H_
#define INC_MODULE_GUI_BASIC_H_

#include "components/comp_base.h"

//Forward Declaration
class TCompTransform;
class TCompCamera;

#define RSTATE_DISABLED	-1.f
#define RSTATE_ENABLED	0.f
#define RSTATE_OVER		1.f
#define RSTATE_CLICKED	2.f
#define RSTATE_RELEASED	3.f

#define GUI_MAX_ROW	20
#define GUI_MAX_COL	20

typedef struct GuiMatrix { CHandle elem[GUI_MAX_ROW][GUI_MAX_COL]; } GuiMatrix;

class TCompGui : public TCompBase {
	std::string menu_name;
	int row, col;
	float render_state;
	float render_target;
	float render_speed;
public:
	static std::map<std::string, GuiMatrix> gui_screens;
	static void addGuiElement(std::string menu_name, int row, int column, CHandle h_gui);
	static void clearScreen(std::string menu_name);
	void update(float elapsed);
	float getRenderState() { return render_state; }
	void setRenderTarget(float rs_target, float speed);
	void setRenderState(float rs_state);
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
	void renderInMenu();
};

#endif
