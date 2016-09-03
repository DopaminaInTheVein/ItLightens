#ifndef INC_MODULE_GUI_BASIC_H_
#define INC_MODULE_GUI_BASIC_H_

#include "components/comp_base.h"

//Forward Declaration
class TCompTransform;
class TCompCamera;

class TCompGui : public TCompBase {
	float render_state;
	float render_target;
	float render_speed;

public:
	void update(float elapsed);
	float getRenderState() { return render_state; }
	void setRenderTarget(float rs_target, float speed);
	void setRenderState(float rs_state);
	bool load(MKeyValue& atts);
	void renderInMenu();
};

#endif
