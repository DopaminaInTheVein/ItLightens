#ifndef INC_COMPONENT_CAMERA_MAIN_H_
#define INC_COMPONENT_CAMERA_MAIN_H_

#include "comp_camera.h"

#define MAX_GUIDE_POINTS 64

// ------------------------------------
struct TCompCameraMain : public TCompCamera {
	bool detect_colsions;
	float smoothDefault;
	float smoothCurrent;

	float hitRadius = 0.1f;
	float hitDistance = 0.1f;

	VEC3 origin_camera;
	VEC3 last_pos_camera;

	//Camera_main unique
	static ClHandle prev_camera_main;

	//Guided camera
	ClHandle guidedCamera;
	TCompTransform * transform;
	//bool guidedCamera;
	//int lastguidedCamPoint;
	//VEC3 guided_positions[MAX_GUIDE_POINTS];
	//CQuaternion guided_rotations[MAX_GUIDE_POINTS];
	void init();
	bool load(MKeyValue& atts);
	bool getUpdateInfo() override;
	void update(float dt);
	bool checkColision(const VEC3& pos, const float smoothCurrent);
	void onGuidedCamera(const TMsgGuidedCamera&);
	void onCreate(const TMsgEntityCreated&);
};

#endif
