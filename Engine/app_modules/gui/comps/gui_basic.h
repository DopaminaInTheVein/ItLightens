#ifndef INC_MODULE_GUI_BASIC_H_
#define INC_MODULE_GUI_BASIC_H_

#include "components/comp_base.h"

#include "../gui_utils.h"

#include <stack>

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
typedef struct GuiListener {
	CHandle handle;
	std::string event_name;
} GuiListener;

class TCompGui : public TCompBase {
	static std::map<std::string, GuiMatrix> gui_screens;
	static std::stack<CHandle> cursors;

	//Menu info
	std::string menu_name;
	int row, col;

	// Size
	float width, height;

	// Render state float for render manager
	float render_state;
	float render_target;
	float render_speed;

	VEC4 color;
	VEC4 origin_color;
	VEC4 target_color;
	float color_speed = 0.0f;
	float color_speed_lag = 0.0f;
	bool loop_color = false;
	VEC4 delta_color;

	// Text coords limits
	RectNormalized text_coords;
	int num_words_per_line;

	//Aux
	void updateColor(float elapsed);
	void updateColorLag(float elapsed);
public:
	// GuiMarix elements
	static CHandle getMatrixHandle(std::string menu_name, int row, int col);
	static GuiMatrix getGuiMatrix(std::string menu_name);
	static void addGuiElement(std::string menu_name, int row, int column, CHandle h_gui);
	static void clearScreen(std::string menu_name);

	// Cursors
	static CHandle getCursor();
	static void pushCursor(CHandle);

	void onCreate(const TMsgEntityCreated&);

	// Menu gets
	int GetRow();
	int GetCol();

	// Size
	float GetWidth();
	float GetHeight();
	void SetWidth(float);
	void SetHeight(float);

	void SetColor(VEC4 new_color) {
		color = new_color;
		origin_color = color;
		loop_color = false;
	}

	bool load(MKeyValue& atts);
	void update(float elapsed);
	void uploadCtes();
	float getRenderState() { return render_state; }
	void setRenderTarget(float rs_target, float speed);
	void setRenderState(float rs_state);
	void setTargetColorAndSpeed(VEC4 new_t_color, float new_color_speed, float new_color_lag = 0.0f, bool loop = false) {
		assert(new_color_speed > 0.0f);
		target_color = new_t_color;
		color_speed = new_color_speed;
		color_speed_lag = new_color_lag;
		loop_color = loop;
		if (!loop) delta_color = color_speed * (target_color - color);
	}
	RectNormalized getTxCoords();
	void setTxCoords(RectNormalized);
	void renderInMenu();
};

#endif
