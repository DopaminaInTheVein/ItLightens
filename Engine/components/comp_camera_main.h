#ifndef INC_COMPONENT_CAMERA_MAIN_H_
#define INC_COMPONENT_CAMERA_MAIN_H_

#include "comp_camera.h"

#define MAX_GUIDE_POINTS 64

// ------------------------------------
struct TCompCameraMain : public TCompCamera {
	bool detect_colsions;
	float smoothDefault;
	float smoothCurrent;

	VEC3 origin_camera;

	//Guided camera
	CHandle guidedCamera;
	TCompTransform * transform;
	//bool guidedCamera;
	//int lastguidedCamPoint;
	//VEC3 guided_positions[MAX_GUIDE_POINTS];
	//CQuaternion guided_rotations[MAX_GUIDE_POINTS];
	bool load(MKeyValue& atts);
	bool getUpdateInfo() override;
	void update(float dt);
	bool checkColision(const VEC3& pos);
	void onGuidedCamera(const TMsgGuidedCamera&);
};

#endif
