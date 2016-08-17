#ifndef INC_CAMERA_H_
#define INC_CAMERA_H_

#include "geometry/geometry.h"
#include "components/comp_guided_camera.h"
#include "components/comp_transform.h"

class CCamera {
	VEC3  position;
	VEC3  target;
	VEC3  front;
	VEC3  up;
	VEC3  left;
	VEC3  up_aux;
	MAT44 view;

	MAT44 projection;
	float fov_vertical_rads;
	float aspect_ratio;
	float zfar;
	float znear;

	bool  is_ortho;

	MAT44 view_projection;
	void updateViewProjection();

public:

	//For Test
	//static float cui_left, cui_right, cui_bottom, cui_top;

	CCamera();

	// View
	void lookAt(VEC3 eye, VEC3 target, VEC3 up = VEC3(0, 1, 0));
	void smoothLookAt(VEC3 eye, VEC3 target, VEC3 up = VEC3(0, 1, 0), float smooth_factor = 10);
	void smoothUpdateInfluence(TCompTransform * tmx, TCompGuidedCamera * gc, int influencia, VEC3 up = VEC3(0, 1, 0));
	void applyQuat(CQuaternion quat, VEC3 p, VEC3 s);

	MAT44 getView() const { return view; }
	VEC3  getPosition() const { return position; }
	VEC3  getFront() const { return front; }
	VEC3  getUp() const { return up; }
	VEC3  getUpAux() const { return up_aux; }   // 0,1,0
	VEC3  getLeft() const { return left; }
	VEC3  getRight() const { return -left; }

	// Projection
	void setProjection(float fov_v_vertical_rads, float znear, float zfar);
	void setAspectRatio(float new_ratio);
	void setOrtho(float size_x, float size_y, float znear, float zfar, float ar = 1);
	bool isOrtho() const { return is_ortho; }

	MAT44 getProjection() const { return projection; }
	float getAspectRatio() const { return aspect_ratio; }
	float getZFar() const { return zfar; }
	float getZNear() const { return znear; }
	float getFov() const { return fov_vertical_rads; }

	MAT44 getViewProjection() const { return view_projection; }
};

#endif
