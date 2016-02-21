#include "mcv_platform.h"
#include "murmur3.h"
#include "imgui/imgui.h"
#include <windows.h>

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
	MurmurHash3_x86_32(text, strlen(text), 12345, &out);
	return out;
}

float getDeltaTime() {
	return ImGui::GetIO().DeltaTime;
}

float squared(float i) {
	return i*i;
}

float realDist(VEC3 init, VEC3 dest) {
	return sqrtf(squaredDist(init, dest));
}
float realDistXZ(VEC3 init, VEC3 dest) {
	return sqrtf(squaredDistXZ(init, dest));
}

float squaredDist(VEC3 init, VEC3 dest) {
	return squared(abs(init.x - dest.x)) + squared(abs(init.y - dest.y)) + squared(abs(init.z - dest.z));
}
float squaredDistXZ(VEC3 init, VEC3 dest) {
	return squared(abs(init.x - dest.x)) + squared(abs(init.z - dest.z));
}

float simpleDist(VEC3 init, VEC3 dest) {
	return abs(init.x - dest.x) + abs(init.y - dest.y) + abs(init.z - dest.z);
}

float simpleDistXZ(VEC3 init, VEC3 dest) {
	return abs(init.x - dest.x) + abs(init.z - dest.z);
}