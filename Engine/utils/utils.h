#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "geometry/geometry.h"
#include "imgui/imgui.h"
#include "windows/app.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

#ifdef NDEBUG

#define dbg(...)

#else

void		dbg(const char* fmt, ...);

#endif

#define MOD_YAW(a) ((a > deg2rad(180.0f)) ? (a - deg2rad(360.0f)) : ((a < deg2rad(-180.0f)) ? (a + deg2rad(360.0f)) : a))
#define assignValueToVar(name, data_map) assingValueFromMap(&name, #name, data_map)

using namespace rapidjson;

void assingValueFromMap(float *variable, char *name, std::map<std::string, float> data_map);

bool		fatal(const char* fmt, ...);
uint32_t	getID(const char* text);

float		getDeltaTime(float always = false);

float squared(float i);

float realDist(const VEC3& init, const VEC3& dest);
float realDistXZ(const VEC3& init, const VEC3& dest);

float squaredDist(const VEC3& init, const VEC3& dest);
float squaredDistXZ(const VEC3& init, const VEC3& dest);
float squaredDistY(const VEC3& init, const VEC3& dest);

float simpleDistXZ(const VEC3& init, const VEC3& dest);
float simpleDist(const VEC3& init, const VEC3& dest);

void getRotationMatrix(const float roll, const float pitch, const float yaw, MAT44 &R);
VEC3 productVectorMatrix(const VEC4& vec, const MAT44& matrix);
VEC3 rotateAround(const VEC3 &pos, const float roll, const float pitch, const float yaw);

///  Performs a linear interpolation between two vectors. (@p v1 toward @p v2)
///  return					The result vector. [(x, y, z)]
///  @param[in]		start	The starting vector.
///  @param[in]		end		The destination vector.
///	 @param[in]		factor	The interpolation factor. [Limits: 0 <= value <= 1.0]
CQuaternion interpolate(const CQuaternion &start, const CQuaternion &end, const float factor);

//template<class TObj>
typedef uint32_t TTagID;
bool isInVector(std::vector<TTagID>& v, TTagID obj);

std::map<std::string, float> readIniFileAttrMap(char* elements_to_read);

std::vector<std::string> list_files_recursively(std::string folder_path);

Document readJSONAtrFile(const std::string route);
std::map<std::string, float> readIniAtrData(const std::string route, std::string atr_to_read);
void writeIniAtrData(const std::string route, std::string element_name, std::map<std::string, float> element_values);

//Check nan VEC3
bool isNan(VEC3 vec);
bool isValid(VEC3 vec);
bool isNormal(VEC3 vec);
#endif
