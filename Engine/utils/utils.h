#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "geometry/geometry.h"
#include "imgui/imgui.h"
#include "windows/app.h"

#include "handle/handle.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "text_encode.h"

#define STRING(s) #s

#define VEC3_VALUES(name) name.x, name.y, name.z
#define VEC4_VALUES(name) name.x, name.y, name.z, name.w

#ifdef NDEBUG

#define dbg(...)
#define		DEBUG_CODE (false)
#define		DEBUG_COMP(comp) DEBUG_CODE
#else
void		dbg(const char* fmt, ...);
#define		DEBUG_CODE (TCompBase::debugging)
#define		DEBUG_COMP(comp) (comp && comp->debugging)
#define		DEBUG_HCOMP(h) (h.isValid() && h.getDebug() && *h.getDebug())
#endif

#define PI 3.14159265359
#define MOD_YAW(a) ((a > deg2rad(180.0f)) ? (a - deg2rad(360.0f)) : ((a < deg2rad(-180.0f)) ? (a + deg2rad(360.0f)) : a))
#define assignValueToVar(name, data_map) assingValueFromMap(&name, #name, data_map)

#define TASKLIST_PICKUP_PILA 5
#define TASKLIST_LEAVE_PILA_ON_CHARGER 6
#define TASKLIST_BREAK_WALL 7
#define TASKLIST_ELEVATOR_DOWN 8
#define TASKLIST_CHARGE_PILA 9
#define TASKLIST_LEAVE_PILA 10
#define TASKLIST_CREATE_BOMB 11

using namespace rapidjson;

void assingValueFromMap(float *variable, char *name, std::map<std::string, float> data_map);

bool		fatal(const char* fmt, ...);
uint32_t	getID(const char* text);

float		getDeltaTime(float always = false);
float random(float vmin, float vmax);

bool isInRoom(CHandle handle);

float squared(float i);

float distY(const VEC3& init, const VEC3& dest);
float realDist(const VEC3& init, const VEC3& dest);
float realDistXZ(const VEC3& init, const VEC3& dest);

float squaredDist(const VEC3& init, const VEC3& dest);
float squaredDistXZ(const VEC3& init, const VEC3& dest);
float squaredDistY(const VEC3& init, const VEC3& dest);

float simpleDistXZ(const VEC3& init, const VEC3& dest);
float simpleDist(const VEC3& init, const VEC3& dest);
bool inSquaredRangeXZ_Y(const VEC3& init, const VEC3& dest, const float& xz, const float& y);

void getRotationMatrix(const float roll, const float pitch, const float yaw, MAT44 &R);
VEC3 productVectorMatrix(const VEC4& vec, const MAT44& matrix);
VEC3 rotateAround(const VEC3 &pos, const float roll, const float pitch, const float yaw);
void rotate_vector_by_quaternion(const VEC3& vec, const CQuaternion& q, VEC3& vprime);
float angleBetween(const VEC3&, const VEC3&);

VEC3 movementUnion(const VEC3& u, const VEC3& v);
bool isZero(VEC3);
void clampVector(VEC3& u, const VEC3& min, const VEC3& max);
///  Performs a linear interpolation between two vectors. (@p v1 toward @p v2)
///  return					The result vector. [(x, y, z)]
///  @param[in]		start	The starting vector.
///  @param[in]		end		The destination vector.
///	 @param[in]		factor	The interpolation factor. [Limits: 0 <= value <= 1.0]
CQuaternion interpolate(const CQuaternion &start, const CQuaternion &end, const float factor);

// STRING TO COLOR
ImColor obtainColorFromString(std::string color);
VEC4 obtainColorNormFromString(std::string color);

//template<class TObj>
typedef uint32_t TTagID;
bool isInVector(std::vector<TTagID>& v, TTagID obj);

std::map<std::string, float> readIniFileAttrMap(char* elements_to_read);

std::vector<std::string> list_files_recursively(std::string folder_path);

Document readJSONAtrFile(const std::string route);
std::map<std::string, float> readIniAtrData(const std::string route, std::string atr_to_read);
std::map<std::string, std::string> readIniAtrDataStr(const std::string route, std::string atr_to_read);
void writeIniAtrData(const std::string route, std::string element_name, std::map<std::string, float> element_values);
void writeIniAtrDataStr(const std::string route, std::string element_name, std::map<std::string, std::string> element_values);
std::map< std::string, std::map<std::string, float> > readAllAtrMaps(const std::string route);

//Math basic
float max(int nargs, ...);
float min(int nargs, ...);
VEC4 mul(VEC4 input, MAT44 transform);
float inverseFloat(float value);
//Check nan VEC3
bool isNan(VEC3 vec);
bool isValid(VEC3 vec);
bool isNormal(VEC3 vec);
void mod(int& value, int module);

//STD operations
#define removeFromVector(v, elem) v.erase(std::remove(v.begin(), v.end(), elem), v.end())
#define setContains(container, elem) (container.find(elem) != container.end())

//Handles, messages, ...
#define BROADCAST_MSG(Msg) getHandleManager<CEntity>()->each([](CEntity * e) {e->sendMsg(Msg());});

#endif