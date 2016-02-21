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

float realDist(VEC3 init, VEC3 dest);
float realDistXZ(VEC3 init, VEC3 dest);

float squaredDist(VEC3 init, VEC3 dest);
float squaredDistXZ(VEC3 init, VEC3 dest);

float simpleDistXZ(VEC3 init, VEC3 dest);
float simpleDist(VEC3 init, VEC3 dest);

#endif
