#ifndef INC_COMPONENT_CAMERA_H_
#define INC_COMPONENT_CAMERA_H_

#include "comp_base.h"
#include "camera/camera.h"

// ------------------------------------
struct TCompCamera : public CCamera, public TCompBase {
	void render() const;
	void update(float dt);
	bool load(MKeyValue& atts);
	void renderInMenu();
};

#endif
