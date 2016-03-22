#include "mcv_platform.h"
#include "murmur3.h"
#include "imgui/imgui.h"
#include <windows.h>
#include <algorithm>

#ifndef NDEBUG

void dbg(const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	char dest[1024 * 16];
	_vsnprintf(dest, sizeof(dest), format, argptr);
	va_end(argptr);
	::OutputDebugString(dest);
}
#endif

bool fatal(const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	char dest[1024 * 16];
	_vsnprintf(dest, sizeof(dest), format, argptr);
	va_end(argptr);
	::OutputDebugString("FATAL!!!\n");
	::OutputDebugString(dest);
	return false;
}

uint32_t getID(const char* text) {
	uint32_t out;
	int stringsize = (int)strlen(text);
	MurmurHash3_x86_32(text, stringsize, 12345, &out);
	return out;
}

float _deltaTimePrev = 1.0f / 60.0f;
float getDeltaTime() {
	float dt = ImGui::GetIO().DeltaTime;
	if (dt > 0.5f) {
		dt = _deltaTimePrev;
	}
	else {
		_deltaTimePrev = dt;
	}
	return dt;
}

float squared(float i) {
	return i*i;
}

float realDist(const VEC3 &init, const VEC3 &dest) {
	return sqrtf(squaredDist(init, dest));
}
float realDistXZ(const VEC3 &init, const VEC3 &dest) {
	return sqrtf(squaredDistXZ(init, dest));
}

float squaredDist(const VEC3& init, const VEC3& dest) {
	return squared(abs(init.x - dest.x)) + squared(abs(init.y - dest.y)) + squared(abs(init.z - dest.z));
}
float squaredDistXZ(const VEC3& init, const VEC3& dest) {
	return squared(abs(init.x - dest.x)) + squared(abs(init.z - dest.z));
}

float squaredDistY(const VEC3& init, const VEC3& dest) {
	return squared(init.y - dest.y);
}

float simpleDist(const VEC3& init, const VEC3& dest) {
	return abs(init.x - dest.x) + abs(init.y - dest.y) + abs(init.z - dest.z);
}

float simpleDistXZ(const VEC3 &init, const VEC3 &dest) {
	return abs(init.x - dest.x) + abs(init.z - dest.z);
}

//return MAT44 rotation from roll, yaw, pitch
void getRotationMatrix(float roll, float pitch, float yaw, MAT44 &R)
{
	MAT44 Rx, Ry, Rz;
	Rx = MAT44::Identity;
	Ry = MAT44::Identity;
	Rz = MAT44::Identity;

	Rx(3, 3) = 0;
	Ry(3, 3) = 0;
	Rz(3, 3) = 0;

	pitch = -pitch;
	yaw = -yaw;
	roll = -roll;
	// Rotation over X axis
	Rx(1, 1) = cos(-pitch*(float)M_PI / 180);
	Rx(1, 2) = -sin(-pitch*(float)M_PI / 180);
	Rx(2, 1) = sin(-pitch*(float)M_PI / 180);
	Rx(2, 2) = cos(-pitch*(float)M_PI / 180);

	// Rotation over Y axis
	Ry(0, 0) = cos(-yaw*(float)M_PI / 180);
	Ry(0, 2) = sin(-yaw*(float)M_PI / 180);
	Ry(2, 0) = -sin(-yaw*(float)M_PI / 180);
	Ry(2, 2) = cos(-yaw*(float)M_PI / 180);

	// Rotation over X axis
	Rz(0, 0) = cos(-roll*(float)M_PI / 180);
	Rz(0, 1) = -sin(-roll*(float)M_PI / 180);
	Rz(1, 0) = sin(-roll*(float)M_PI / 180);
	Rz(1, 1) = cos(-roll*(float)M_PI / 180);

	// Rotation matrix R
	R = Rx*Ry*Rz;
}

VEC3 productVectorMatrix(const VEC4& vec, const MAT44& matrix) {
	VEC3 res;
	res.x = vec.x*matrix(0, 0) + vec.y*matrix(0, 1) + vec.z*matrix(0, 2);
	res.y = vec.x*matrix(1, 0) + vec.y*matrix(1, 1) + vec.z*matrix(1, 2);
	res.z = vec.x*matrix(2, 0) + vec.y*matrix(2, 1) + vec.z*matrix(2, 2);
	return res;
}

VEC3 rotateAround(const VEC3 &pos, const float roll, const float pitch, const float yaw)
{
	if (yaw == 0.0f && pitch == 0.0f && roll == 0.0f)
		return pos;

	MAT44 rotMatrix;
	getRotationMatrix(roll, pitch, yaw, rotMatrix);

	VEC4 pos4 = VEC4(pos.x, pos.y, pos.z, 0);
	VEC3 pos_f = productVectorMatrix(pos4, rotMatrix);

	return pos_f;
}

//template<class TObj>
#include "components\entity_tags.h"
//template<class  TObj>
bool isInVector(std::vector<TTagID>& v, TTagID obj)
{
	return (std::find(v.begin(), v.end(), obj) != v.end());
}
