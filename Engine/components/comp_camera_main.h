#ifndef INC_COMPONENT_CAMERA_MAIN_H_
#define INC_COMPONENT_CAMERA_MAIN_H_

#include "comp_camera.h"

#define MAX_GUIDE_POINTS 64

struct collision_data {
	VEC3 dir;
	float dist;
	bool intersection = false;
};

// ------------------------------------
struct TCompCameraMain : public TCompCamera {
private:
	void endGuidedCamera();
public:
	bool detect_colsions;
	bool manual_control = false;
	float smoothDefault;
	float smoothCurrent;

	float hitRadius = 0.1f;
	float hitDistance = 0.5f;
	float hitDistanceMin = 0.2f;

	VEC3 origin_camera;
	VEC3 last_pos_camera;

	//Camera_main unique
	static CHandle prev_camera_main;

	//Guided camera
	CHandle guidedCamera;
	TCompTransform * transform;
	//bool guidedCamera;
	//int lastguidedCamPoint;
	//VEC3 guided_positions[MAX_GUIDE_POINTS];
	//CQuaternion guided_rotations[MAX_GUIDE_POINTS];
	void init();
	bool load(MKeyValue& atts);
	bool getUpdateInfo() override;
	void update(float dt);
	bool checkColision(const VEC3& pos, const float distanceToTarget, collision_data& result);
	bool getPosIfColisionClipping(const VEC3& pos, collision_data& result);
	bool getManualControl() { return manual_control; }
	void setManualControl(bool control) { manual_control = control; }
	void onGuidedCamera(const TMsgGuidedCamera&);
	void onCreate(const TMsgEntityCreated&);
	void skipCinematic();
	void reset();
	void StopCinematic();
};

#endif
