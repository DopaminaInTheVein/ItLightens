#ifndef INC_LOGIC_SBB_H
#define INC_LOGIC_SBB_H

using namespace std;

class CNavmesh;
class CHandle;
class bt_mole;
class guard_alert;

class SBB
{
private:
	static map<string, CNavmesh> sbbNavs;
	static map<string, bool > sbbBool;
	static map<string, int > sbbInt;
	static map<string, VEC3> sbbVEC3;
	static map<string, CHandle> sbbHandle;
	static map<string, bt_mole*> sbbMole;
	static map<string, vector<CHandle> > sbbHandlesVector;
	static map<string, guard_alert> sbbGuardAlerts;

public:
	SBB() = delete;
	static void init();
	static void update(float);

	// NavMesh
	static void postNavmesh(string, CNavmesh);
	static CNavmesh readNavmesh(string);
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
	static void postHandle(string, CHandle);
	static CHandle readHandle(string);
	// sbbAiMole
	static void postMole(string, bt_mole*);
	static bt_mole* readMole(string);
	// sbbHandleVector
	static void postHandlesVector(string, vector<CHandle>);
	static vector<CHandle> readHandlesVector(string);
	// sbbGuardAlerts
	static map<string, guard_alert> getGuardAlerts() { return sbbGuardAlerts; }
	static void postGuardAlert(string, guard_alert);
	static guard_alert readGuardAlert(string);
	static void updateGuardAlerts(float);
};

#endif
