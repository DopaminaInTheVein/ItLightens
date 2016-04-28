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
	VEC3 scale, translation;
	view.Decompose(scale, rotation, translation);
	updateViewProjection();
}

void CCamera::applyQuat(CQuaternion quat) {
	view = MAT44::CreateFromQuaternion(quat);

	front = view.Forward();
	up = view.Up();
	left = view.Left();
	VEC3 scale, translation;
	view.Decompose(scale, rotation, translation);
	assert(rotation == quat);
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
	VEC3 scale, translation;
	view.Decompose(scale, rotation, translation);
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