#ifndef INC_MCV_PLATFORM_H_
#define INC_MCV_PLATFORM_H_

// Global settings
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#define NOMINMAX
#define _WIN32_WINNT      _WIN32_WINNT_WIN7
#define MAX_ENTITIES 128

#define SAFE_RELEASE(p)  if (p) p->Release(), p = nullptr

// C++ includes
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <map>
#include <string>

// Windows

// Engine ...
#include "utils/data_provider.h"
#include "utils/utils.h"
#include "geometry/geometry.h"
#include "render/render.h"

//DEBUG
#include "debug/debug.h"
extern CDebug * Debug;

#endif
