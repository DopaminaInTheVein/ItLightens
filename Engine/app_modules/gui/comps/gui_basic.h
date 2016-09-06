#ifndef INC_MODULE_GUI_BASIC_H_
#define INC_MODULE_GUI_BASIC_H_

#include "components/comp_base.h"

#include "../gui_utils.h"

//Forward Declaration
class TCompTransform;
class TCompCamera;

#define RSTATE_DISABLED	-1.f
#define RSTATE_ENABLED	0.f
#define RSTATE_OVER		1.f
#define RSTATE_CLICKED	2.f
#define RSTATE_RELEASED	3.f

class TCompGui : public TCompBase {
	// Render state float for render manager
	float render_state;
	float render_target;
	float render_speed;

	// Text coords limits
	RectNormalized text_coords;
	int num_words_per_line;

public:
	bool load(MKeyValue& atts);
	void update(float elapsed);
	void uploadCtes();
	float getRenderState() { return render_state; }
	void setRenderTarget(float rs_target, float speed);
	void setRenderState(float rs_state);
	RectNormalized getTxCoords();
	void renderInMenu();
};

#endif
