#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "geometry/geometry.h"
#include "imgui/imgui.h"

#ifdef NDEBUG

#define dbg(...)

#else

void dbg(const char* fmt, ...);

#endif

bool fatal(const char* fmt, ...);
uint32_t getID(const char* text);

float getDeltaTime();

float squared(float i);

float realDist(const VEC3& init, const VEC3& dest);
float realDistXZ(const VEC3& init, const VEC3& dest);

float squaredDist(const VEC3& init, const VEC3& dest);
float squaredDistXZ(const VEC3& init, const VEC3& dest);

float simpleDistXZ(const VEC3& init, const VEC3& dest);
float simpleDist(const VEC3& init, const VEC3& dest);

void getRotationMatrix(const double roll, const double pitch, const double yaw, MAT44 &R);
VEC3 productVectorMatrix(const VEC4& vec, const MAT44& matrix);
VEC3 rotateAround(const VEC3 &pos, const float roll, const float pitch, const float yaw);

#endif
