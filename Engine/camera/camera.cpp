#include "mcv_platform.h"
#include "camera.h"

CCamera::CCamera()
	: aspect_ratio(1.0f)
{
	lookAt(VEC3(0, 0, 0), VEC3(0, 0, 1));
	setProjection(deg2rad(60.f), 0.1f, 100.f);
}

void CCamera::updateViewProjection() {
	view_projection = view * projection;
}

void CCamera::lookAt(VEC3 new_position, VEC3 new_target, VEC3 new_up_aux) {
	position = new_position;
	target = new_target;
	up_aux = new_up_aux;

	view = MAT44::CreateLookAt(position, target, up_aux);

	front = view.Forward();
	up = view.Up();
	left = view.Left();

	// CORRECT?
	rotation.w = sqrtf(1.0 + view._11 + view._22 + view._33) / 2.0;
	double w4 = (4.0 * rotation.w);
	rotation.x = (view._32 - view._23) / w4;
	rotation.y = (view._13 - view._31) / w4;
	rotation.z = (view._21 - view._12) / w4;

	updateViewProjection();
}

void CCamera::applyQuat(CQuaternion quat) {
	// INCORRECT

	MAT44 apply(
		1.0f - 2.0f*quat.y*quat.y - 2.0f*quat.z*quat.z, 2.0f*quat.x*quat.y - 2.0f*quat.z*quat.w, 2.0f*quat.x*quat.z + 2.0f*quat.y*quat.w, 0.0f,
		2.0f*quat.x*quat.y + 2.0f*quat.z*quat.w, 1.0f - 2.0f*quat.x*quat.x - 2.0f*quat.z*quat.z, 2.0f*quat.y*quat.z - 2.0f*quat.x*quat.w, 0.0f,
		2.0f*quat.x*quat.z - 2.0f*quat.y*quat.w, 2.0f*quat.y*quat.z + 2.0f*quat.x*quat.w, 1.0f - 2.0f*quat.x*quat.x - 2.0f*quat.y*quat.y, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	view *= apply;
	front = view.Forward();
	up = view.Up();
	left = view.Left();
	// CORRECT?
	rotation.w = sqrtf(1.0 + view._11 + view._22 + view._33) / 2.0;
	double w4 = (4.0 * rotation.w);
	rotation.x = (view._32 - view._23) / w4;
	rotation.y = (view._13 - view._31) / w4;
	rotation.z = (view._21 - view._12) / w4;
	//assert(rotation == quat);
	updateViewProjection();
}

void CCamera::smoothLookAt(VEC3 new_position, VEC3 new_target, VEC3 new_up_aux) {
	float drag = getDeltaTime() * 10;
	float drag_i = 1 - drag;

	position = new_position*drag + position*drag_i;
	target = new_target*drag + target*drag_i;
	up_aux = new_up_aux;

	view = MAT44::CreateLookAt(position, target, up_aux);

	front = view.Forward();
	up = view.Up();
	left = view.Left();
	// CORRECT?
	rotation.w = sqrtf(1.0 + view._11 + view._22 + view._33) / 2.0;
	double w4 = (4.0 * rotation.w);
	rotation.x = (view._32 - view._23) / w4;
	rotation.y = (view._13 - view._31) / w4;
	rotation.z = (view._21 - view._12) / w4;
	updateViewProjection();
}

// Projection
void CCamera::setProjection(float new_fov_vertical_rads, float new_znear, float new_zfar) {
	fov_vertical_rads = new_fov_vertical_rads;
	znear = new_znear;
	zfar = new_zfar;
	projection = MAT44::CreatePerspectiveFieldOfView(fov_vertical_rads, aspect_ratio, znear, zfar);
	updateViewProjection();
}

void CCamera::setAspectRatio(float new_ratio) {
	aspect_ratio = new_ratio;
	projection = MAT44::CreatePerspectiveFieldOfView(fov_vertical_rads, aspect_ratio, znear, zfar);
	updateViewProjection();
}