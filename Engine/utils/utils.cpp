#include "mcv_platform.h"
#include "murmur3.h"
#include "imgui/imgui.h"
#include <windows.h>
#include <algorithm>
#include "timer.h"
#include "logic/sbb.h"
#include "components/comp_room.h"
#include "components/entity.h"
#include "windows\app.h"

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
float getDeltaTime(float always) {
	if (GameController->GetGameState() == CGameController::RUNNING || always) {
		CApp& app = CApp::get();
		float dt = app.timer_app.GetDeltaTime();
		if (dt > 0.5f) {
			dt = _deltaTimePrev;
		}
		else {
			_deltaTimePrev = dt;
		}
		if (GameController->GetGameState() == CGameController::STOPPED_INTRO)
			return dt / 2.5f;
#ifndef NDEBUG
		else if (controller->isSlowButtonPressed())
			return dt / 10.f;
#endif
		else
			return dt;
	}

	return 0.0f;
}

float urandom() {
	return (float)rand() / (float)RAND_MAX;
}

float random(float vmin, float vmax) {
	return vmin + urandom() * (vmax - vmin);
}

bool isInRoom(CHandle h) {
	int pjSala = SBB::readSala();
	if (pjSala == -1) return true;
	CEntity * e = h;
	if (!e) return true;
	TCompRoom * room = e->get<TCompRoom>();
	if (!room) return true;
	if (std::find(room->name.begin(), room->name.end(), -1) != room->name.end())  return true;
	return std::find(room->name.begin(), room->name.end(), pjSala) != room->name.end();
}

float squared(float i) {
	return i*i;
}
float distY(const VEC3 &init, const VEC3 &dest) {
	return abs(init.y - dest.y);
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
bool inSquaredRangeXZ_Y(const VEC3& init, const VEC3& dest, const float& squared_xz, const float& y) {
	if (abs(init.y - dest.y) > y) return false;
	float dist = squaredDistXZ(init, dest);
	return  dist < squared_xz;
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

void rotate_vector_by_quaternion(const VEC3& vec, const CQuaternion& q, VEC3& vprime) {
	CQuaternion inv;
	//q.Inverse(inv);
	q.Conjugate(inv);

	CQuaternion qt = q * CQuaternion(vec.x, vec.y, vec.z, 0.0f) * inv;

	vprime = VEC3(qt.x, qt.y, qt.z);
}

float angleBetween(const VEC3& u, const VEC3& v)
{
	float dot = u.Dot(v);
	return acosf(dot / (v.Length() * u.Length()));
}

VEC3 movementUnion(const VEC3& u, const VEC3& v)
{
	VEC3 res = u;
	//if (u.x != 0.f && v.x != 0.f) assert(sameSign(u.x, v.x));
	//if (u.y != 0.f && v.y != 0.f) assert(sameSign(u.y, v.y));
	//if (u.z != 0.f && v.z != 0.f) assert(sameSign(u.z, v.z));
	if (abs(u.x) < abs(v.x)) res.x = v.x;
	if (abs(u.y) < abs(v.y)) res.y = v.y;
	if (abs(u.z) < abs(v.z)) res.z = v.z;
	return res;
}

bool isZero(VEC3 u)
{
	return u.x == 0.f && u.y == 0.f && u.z == 0.f;
}

void clampVector(VEC3& u, const VEC3& min, const VEC3& max)
{
	clamp_me(u.x, min.x, max.x);
	clamp_me(u.y, min.y, max.y);
	clamp_me(u.z, min.z, max.z);
}

//template<class TObj>
#include "components\entity_tags.h"
//template<class  TObj>
bool isInVector(std::vector<TTagID>& v, TTagID obj)
{
	return (std::find(v.begin(), v.end(), obj) != v.end());
}

// Assigns to a variable the value of the map that corresponds to the specified name
void assingValueFromMap(float *variable, char *name, std::map<std::string, float> data_map) {
	*variable = data_map[name];
}

// Reads and parses a JSON document
Document readJSONAtrFile(const std::string route) {
	FILE* pFile = fopen(route.c_str(), "rb");
	char buffer[65536];
	FileReadStream is(pFile, buffer, sizeof(buffer));
	Document document;
	document.ParseStream<0, UTF8<>, FileReadStream>(is);
	fclose(pFile);

	return document;
}

// Obtains all the atributes of the specified element of a JSON object
std::map<std::string, float> readIniAtrData(const std::string route, std::string element) {
	Document document = readJSONAtrFile(route);
	std::map<std::string, float> atributes;
	if (document.HasMember(element.c_str())) {
		for (rapidjson::Value::ConstMemberIterator it = document[element.c_str()].MemberBegin(); it != document[element.c_str()].MemberEnd(); ++it) {
			atributes[it->name.GetString()] = it->value.GetFloat();
		}
	}
	return atributes;
}

// Obtains all the atributes of the specified element of a JSON object
std::map<std::string, std::string> readIniAtrDataStr(const std::string route, std::string element) {
	Document document = readJSONAtrFile(route);
	std::map<std::string, std::string> atributes;

	for (rapidjson::Value::ConstMemberIterator it = document[element.c_str()].MemberBegin(); it != document[element.c_str()].MemberEnd(); ++it) {
		atributes[it->name.GetString()] = it->value.GetString();
	}

	return atributes;
}

// Modifies the specified json element of the specified file
void writeIniAtrData(const std::string route, std::string element, std::map<std::string, float> element_values) {
	Document document = readJSONAtrFile(route);
	for (auto atribute : element_values) {
		document[element.c_str()][atribute.first.c_str()].SetFloat(atribute.second);
	}

	FILE* pFile = fopen(route.c_str(), "wb");
	char buffer[65536];
	FileWriteStream os(pFile, buffer, sizeof(buffer));
	PrettyWriter<FileWriteStream> prettywritter(os);
	document.Accept(prettywritter);
	fclose(pFile);
}

// Lists all files contained in the specified folder recursively
std::vector<std::string> list_files_recursively(std::string folder_path) {
	std::vector<std::string> files;
	char search_path[200];
	sprintf(search_path, "%s/*.*", folder_path.c_str());
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			std::string filename(fd.cFileName);
			// if the entry is a file, we add it
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				files.push_back(std::string(folder_path) + "/" + fd.cFileName);
			}
			// if the entry is a directory, we call the function recursively
			else if (filename.compare(".") != 0 && filename.compare("..")) {
				std::string subfolder_route = folder_path + "/" + fd.cFileName;
				std::vector<std::string> files_subfolder = list_files_recursively(subfolder_route);
				for (auto file : files_subfolder) {
					files.push_back(file);
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	return files;
}

CQuaternion interpolate(const CQuaternion &start, const CQuaternion &end, const float factor) {
	// CORRECT?
	if (factor <= 0.0f) {
		return start;
	}
	else if (factor >= 1.0f) {
		return end;
	}

	CQuaternion result;

	result.x = start.x + (end.x - start.x)*factor;
	result.y = start.y + (end.y - start.y)*factor;
	result.z = start.z + (end.z - start.z)*factor;
	result.w = start.w + (end.w - start.w)*factor;

	return result;
}

// COLOR

ImColor obtainColorFromString(std::string color) {
	int red = stoi(color.substr(1, 2), 0, 16);
	int green = stoi(color.substr(3, 2), 0, 16);
	int blue = stoi(color.substr(5, 2), 0, 16);
	int alpha = stoi(color.substr(7, 2), 0, 16);

	return ImColor(red, green, blue, alpha);
}

// Basic math
float max(int nargs, ...) {
	float max_value = FLT_MIN;
	va_list listPointer;
	va_start(listPointer, nargs);
	for (int i = 0; i < nargs; i++)
	{
		float arg = va_arg(listPointer, int);
		if (arg > max_value) max_value = arg;
	}
	return max_value;
}

float min(int nargs, ...) {
	float min_value = FLT_MAX;
	va_list listPointer;
	va_start(listPointer, nargs);
	for (int i = 0; i < nargs; i++)
	{
		float arg = va_arg(listPointer, int);
		if (arg < min_value) min_value = arg;
	}
	return min_value;
}

//Check Nan, Infinity's zero values,...

//Is nan?
bool isNan(VEC3 vec) {
	return (vec.x != vec.x)
		|| (vec.y != vec.y)
		|| (vec.z != vec.z);
}

//Is not nan either infinity?
bool isValid(VEC3 vec) {
	return !isNan(vec)
		&& (isfinite(vec.x))
		&& (isfinite(vec.y))
		&& (isfinite(vec.z));
}

// Is valid and different zero?
bool isNormal(VEC3 vec) {
	return isValid(vec) && vec.LengthSquared() > 0;
}