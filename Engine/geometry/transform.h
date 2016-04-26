#ifndef INC_GEOMETRY_TRANSFORM_H_
#define INC_GEOMETRY_TRANSFORM_H_

#include "geometry.h"


//extern CDebug* Debug;

// ---------------------------
class CTransform {
	CQuaternion rotation;
	VEC3        position;
	VEC3        scale;
public:
	// ---------------------------
	CTransform() : scale(1.f, 1.f, 1.f) {}
	MAT44       asMatrix() const {
		MAT44 matScale;
		if (scale.x == 1.f && scale.y == 1.f || scale.z == 1.f) {
			matScale = MAT44::Identity;
		}
		else {
			matScale = MAT44::CreateScale(scale);
		}

		MAT44 mat = matScale * MAT44::CreateFromQuaternion(rotation);
		mat.Translation(position);
		return mat;
	}

	CQuaternion getRotation() const { return rotation; }
	VEC3        getPosition() const { return position; }
	VEC3        getScale() const { return scale; }

	// ---------------------------
	// Yes, these are not super efficient
	VEC3        getFront() const {
		PROFILE_FUNCTION("apply geometry get front");
		VEC3 front;
		front.x = 2 * (rotation.x*rotation.z + rotation.w*rotation.y);
		front.y = 2 * (rotation.y*rotation.z - rotation.w*rotation.x);
		front.z = 1 - 2 * (rotation.x*rotation.x + rotation.y*rotation.y);

		return front;
	}
	VEC3        getLeft() const {
		PROFILE_FUNCTION("apply geometry get left");
		VEC3 left;	

		left.x = 1 - 2 * (rotation.y*rotation.y + rotation.z*rotation.z);
		left.y = 2 * (rotation.x*rotation.y + rotation.w*rotation.z);
		left.z = 2 * (rotation.x*rotation.z - rotation.w*rotation.y);

		return left;
	}

	VEC3			getUp() const {
		PROFILE_FUNCTION("apply geometry get up");
		VEC3 up;

		up.x = 2 * (rotation.x*rotation.y - rotation.w*rotation.z);
		up.y = 1 - 2 * (rotation.x*rotation.x + rotation.z*rotation.z);
		up.z = 2 * (rotation.y*rotation.z + rotation.w*rotation.x);

		return up;
	}

	// ---------------------------
	void       getAngles(float* yaw, float* pitch) const {
		VEC3 f = getFront();
		getYawPitchFromVector(f, yaw, pitch);

	}
	void setAngles(float new_yaw, float new_pitch) {
		rotation = CQuaternion::CreateFromYawPitchRoll(new_yaw, -new_pitch, 0.f);
	}

	float getDeltaYawToAimTo(VEC3 target) const;
	float getDeltaYawToAimDirection(VEC3 direction) const;
	bool isHalfConeVision(VEC3 target, float half_cone_in_rads) const;
	bool isInFront(VEC3 target) const;
	bool isInLeft(VEC3 target) const;

	// ---------------------------
	void        setPosition(VEC3 new_pos) {
		position = new_pos;
	}
	void        setScale(VEC3 new_scale) {
		scale = new_scale;
	}
	void        setRotation(CQuaternion new_rotation) {
		rotation = new_rotation;
		rotation.Normalize();
	}

	void        lookAt(VEC3 eye, VEC3 target, VEC3 up = VEC3(0, 1, 0)) {
		// As the CreateLookAt is used for cameras, which have the +Z
		// to the -front, we use this same function but tweak the params
		// so the +Z is looking to target
		auto delta = target - eye;
		auto m = MAT44::CreateLookAt(eye, eye - delta, up).Invert();
		m.Decompose(scale, rotation, position);
	}
};

#endif
