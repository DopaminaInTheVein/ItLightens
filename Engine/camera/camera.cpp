#include "mcv_platform.h"
#include "camera.h"

//float CCamera::cui_bottom = 0.f;
//float CCamera::cui_top = 10.f;
//float CCamera::cui_left = 0.f;
//float CCamera::cui_right = 10.f;

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
	assert(view.m);
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
void CCamera::setOrtho(float size_x, float size_y, float new_znear, float new_zfar, float ar) {
	//ar = aspect_ratio screen
	znear = new_znear;
	zfar = new_zfar;
	aspect_ratio = (float)size_x / (float)size_y; //aspect_ratio of camera (used?)
	float left = 0.f, bottom = 0.f;
	float right = size_x;
	float top = size_y;

	if (ar > 1) {
		float offset_y = (size_y - (size_y / ar)) * 0.5f;
		top -= offset_y;
		bottom += offset_y;
	}
	projection = MAT44::CreateOrthographicOffCenter(left, right, bottom, top, znear, zfar);
	//projection = MAT44::CreateOrthographicOffCenter(cui_left, cui_right, cui_bottom, cui_top, znear, zfar);
	is_ortho = true;
	//Ojo! Solo funciona para zetas negativas y up (0,1,0)!
	min_ortho = VEC3(left, bottom, -zfar);
	max_ortho = VEC3(right, top, znear);
	updateViewProjection();
}

void CCamera::setAspectRatio(float new_ratio) {
	aspect_ratio = new_ratio;
	projection = MAT44::CreatePerspectiveFieldOfView(fov_vertical_rads, aspect_ratio, znear, zfar);
	updateViewProjection();
}