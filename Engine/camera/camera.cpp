#include "mcv_platform.h"
#include "camera.h"

CCamera::CCamera()
	: aspect_ratio(1.0f)
	, is_ortho(false)
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

	front = (target - position);
	front.Normalize();

	left = up_aux.Cross(front);
	left.Normalize();

	up = front.Cross(left);
	up.Normalize();

	updateViewProjection();
}

void CCamera::applyQuat(CQuaternion quat, VEC3 p, VEC3 s) {
	MAT44 rotationMatrix = MAT44::CreateFromQuaternion(quat);
	rotationMatrix.Translation(p);
	MAT44 scaleMatrix = MAT44::CreateScale(s);

	//view = scaleMatrix * rotationMatrix;
	//view = rotationMatrix;
	MAT44 preview = MAT44::Transform(view, quat);
	view = preview;

	front = view.Forward();
	up = view.Up();
	left = view.Left();

	//assert(rotation == quat);
	updateViewProjection();
}

void CCamera::smoothLookAt(VEC3 new_position, VEC3 new_target, VEC3 new_up_aux, float smooth_factor) {
	float drag = getDeltaTime() * smooth_factor;
	float drag_i = 1 - drag;

	position = new_position*drag + position*drag_i;
	target = new_target*drag + target*drag_i;
	up_aux = new_up_aux;

	view = MAT44::CreateLookAt(position, target, up_aux);

	front = (target - position);
	front.Normalize();

	left = up_aux.Cross(front);
	left.Normalize();

	up = front.Cross(left);
	up.Normalize();

	updateViewProjection();
}

// Projection
void CCamera::setProjection(float new_fov_vertical_rads, float new_znear, float new_zfar) {
	fov_vertical_rads = new_fov_vertical_rads;
	znear = new_znear;
	zfar = new_zfar;
	is_ortho = false;
	projection = MAT44::CreatePerspectiveFieldOfView(fov_vertical_rads, aspect_ratio, znear, zfar);
	updateViewProjection();
}

// Orthographic projection
void CCamera::setOrtho(int xres, int yres, float new_znear, float new_zfar) {
	znear = new_znear;
	zfar = new_zfar;
	setAspectRatio((float)xres / (float)yres);
	//projection = MAT44::CreateOrthographicOffCenter(0, xres, 0, yres, znear, zfar);
	//projection = MAT44::CreateOrthographicOffCenter(0, 10.f, 0, 10.f / aspect_ratio, znear, zfar);
	float left = 0.f, bottom = 0.f;
	float right = 10.f;
	float top = 10.f;
	if (aspect_ratio > 1) {
		float offset_y = top * 0.5f / aspect_ratio;
		bottom += offset_y;
		top -= offset_y;
	}
	projection = MAT44::CreateOrthographicOffCenter(left, right, bottom, top, znear, zfar);
	is_ortho = true;
	updateViewProjection();
}

void CCamera::setAspectRatio(float new_ratio) {
	dbg("Hago set aspectio_ratio = %f", new_ratio);
	aspect_ratio = new_ratio;
	dbg("Ahora aspect_ratio vale = %f", aspect_ratio);

	projection = MAT44::CreatePerspectiveFieldOfView(fov_vertical_rads, aspect_ratio, znear, zfar);
	updateViewProjection();
}