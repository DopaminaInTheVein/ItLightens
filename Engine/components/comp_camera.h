#ifndef INC_COMPONENT_CAMERA_H_
#define INC_COMPONENT_CAMERA_H_

#include "comp_base.h"
#include "camera/camera.h"

class CEntity;
struct TMsgGetCullingViewProj;

#define MAX_GUIDE_POINTS 64

// ------------------------------------
struct TCompCamera : public CCamera, public TCompBase {
	void render() const;
	void update(float dt);
	bool load(MKeyValue& atts);
	void renderInMenu();
	void updateFromEntityTransform(CEntity* e_owner);
	void onGetViewProj(const TMsgGetCullingViewProj& msg);
	VEC3  getMinOrtho() const;
	VEC3  getMaxOrtho() const;
	float* getNearPointer() { return getZNearPointer(); }
	float* getFarPointer() { return getZFarPointer(); }
};

#endif
