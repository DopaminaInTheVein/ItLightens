#ifndef INC_LOGIC_SBB_H
#define INC_LOGIC_SBB_H

//#include "geometry/geometry.h"

using namespace std;

class CNavmesh;
class ClHandle;
class bt_mole;
class guard_alert;

class SBB
{
private:
	static CNavmesh sbbNav;
	static int sbbSala;
	static map<string, bool > sbbBool;
	static map<string, int > sbbInt;
	static map<string, VEC3> sbbVEC3;
	static map<string, ClHandle> sbbHandle;
	static map<string, bt_mole*> sbbMole;
	static map<string, vector<ClHandle> > sbbHandlesVector;
	static map<string, vector<VEC3> > sbbVEC3Vector;

public:
	static map<string, guard_alert> sbbGuardAlerts;

	SBB() = delete;
	static void init();
	static void update(float);

	// Sala
	static void postSala(int);
	static int readSala();

	// NavMesh
	static void postNavmesh(CNavmesh);
	static CNavmesh readNavmesh();
	// sbbInt
	static void postInt(string, int);
	static int readInt(string);
	static void inc(string);
	static void dec(string);
	// sbbBool
	static void postBool(string, bool);
	static bool readBool(string);
	// sbbVEC3
	static void postVEC3(string, VEC3);
	static VEC3 readVEC3(string);
	// sbbHandle
	static void postHandle(string, ClHandle);
	static ClHandle readHandle(string);
	// sbbAiMole
	static void postMole(string, bt_mole*);
	static bt_mole* readMole(string);
	// sbbHandleVector
	static void postHandlesVector(string, vector<ClHandle>);
	static vector<ClHandle> readHandlesVector(string);
	// sbbVEC3Vector
	static void postVEC3Vector(string, vector<VEC3>);
	static vector<VEC3> readVEC3Vector(string);
	static void addVEC3ToVector(string, VEC3);
	static void removeVEC3ToVector(string, VEC3);
	// sbbGuardAlerts
	static void postGuardAlert(string, guard_alert);
	static guard_alert readGuardAlert(string);
	static void updateGuardAlerts(float);
};

#endif
