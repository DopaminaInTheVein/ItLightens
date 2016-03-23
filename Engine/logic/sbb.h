#ifndef INC_LOGIC_SBB_H
#define INC_LOGIC_SBB_H

using namespace std;

class CHandle;
class bt_mole;

class SBB
{
private:
	static map<string, bool > sbbBool;
	static map<string, int > sbbInt;
	static map<string, VEC3> sbbVEC3;
	static map<string, CHandle> sbbHandle;
	static map<string, bt_mole*> sbbMole;
	static map<string, vector<CHandle> > sbbHandlesVector;

public:
	SBB() = delete;
	static void init();
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
};

#endif
