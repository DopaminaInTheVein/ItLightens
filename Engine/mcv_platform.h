#ifndef INC_MCV_PLATFORM_H_
#define INC_MCV_PLATFORM_H_

// -- Global settings -- //
//vi#define FINAL_BUILD
#ifndef FINAL_BUILD //Defines para version NO final

#define TEST_VALUES // Read initialization_test.json
#define IMGUI_INCLUDE_IMGUI_USER_H
//#define CALIBRATE_GAME
//#define SECURE_HANDLES
#else //defines obligatorios version final
#ifndef NDEBUG
void error() { ¡You are trying Compile Final Build on Debug! }
#endif
//#define SECURE_HANDLES
#endif
// -- Final Global settings -- //

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#define NOMINMAX
#define _WIN32_WINNT      _WIN32_WINNT_WIN7
#define MAX_ENTITIES 1000

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
#include <deque>
#include <map>
#include <string>

// Windows

// Engine ...
#include "imgui/imgui.h"
#include "profiling/profiling.h"
#include "utils/data_provider.h"
#include "utils/utils.h"
#include "utils/macros/macros.h"
#include "debug/log.h"

#include "geometry/geometry.h"
#include "render/render.h"
#include "utils/XMLParser.h"
#include "app_modules/gameController.h"

//physx
#include "PxPhysicsAPI.h"

#ifndef NDEBUG
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x64")
#pragma comment(lib, "PhysX3CookingDEBUG_x64.lib")
#pragma comment(lib, "PxTaskDEBUG.lib")
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")

#else
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3Cooking_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib, "PxTask.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")

#endif

//Physics
#include "physx\physx_manager.h"
extern CPhysxManager *g_PhysxManager;

//DEBUG
#include "debug/debug_itlightens.h"
extern CDebug * Debug;

#endif
