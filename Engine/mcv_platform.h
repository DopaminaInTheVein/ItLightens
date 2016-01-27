#ifndef INC_MCV_PLATFORM_H_
#define INC_MCV_PLATFORM_H_

// Global settings
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#define NOMINMAX

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
#include "geometry/geometry.h"
#include "render/render.h"
#include "utils/utils.h"


#endif



