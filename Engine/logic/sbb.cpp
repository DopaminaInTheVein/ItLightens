#include "mcv_platform.h"
#include <windows.h>
#include <string>
#include "sbb.h"
#include "handle/handle.h"
#include "logic/bt_mole.h"
#include "logic/bt_guard.h"
#include "recast/navmesh.h"

int SBB::sbbSala;
CNavmesh SBB::sbbNav;
map<string, int> SBB::sbbInt;
map<string, bool> SBB::sbbBool;
map<string, VEC3> SBB::sbbVEC3;
map<string, bt_mole*> SBB::sbbMole;
map<string, CHandle> SBB::sbbHandle;
map<string, vector<CHandle>> SBB::sbbHandlesVector;
map<string, vector<VEC3> > SBB::sbbVEC3Vector;
map<string, guard_alert> SBB::sbbGuardAlerts;
vector<float> SBB::letterSpacing;

void SBB::init() {
	postBool("possMode", false);
	postVEC3Vector("wb_pos", vector<VEC3>());
}

void SBB::update(float dt) {
	updateGuardAlerts(dt);
}

void SBB::postSala(int sala) {
	sbbSala = sala;
}
int SBB::readSala() {
	return sbbSala;
}

void SBB::postNavmesh(CNavmesh navmesh) {
	sbbNav = navmesh;
}

CNavmesh SBB::readNavmesh() {
	return sbbNav;
}

void SBB::postInt(string name, int value) {
	sbbInt[name] = value;
}

int SBB::readInt(string name) {
	if (sbbInt.find(name) == sbbInt.end())
	{
		fatal("sbbInt: Se intenta leer String que no existe!");
	}
	return sbbInt[name];
}

void SBB::inc(string name) {
	if (sbbInt.find(name) == sbbInt.end())
	{
		fatal("sbbInt: Se intenta incrementar String que no existe!");
	}
	sbbInt[name]++;
}

void SBB::dec(string name) {
	if (sbbInt.find(name) == sbbInt.end())
	{
		fatal("sbbInt: Se intenta decrementar String que no existe!");
	}
	sbbInt[name]--;
}

// sbbBool

void SBB::postBool(string name, bool value) {
	sbbBool[name] = value;
}

bool SBB::readBool(string name) {
	if (sbbBool.find(name) == sbbBool.end())
	{
		fatal("sbbBool: Se intenta leer String que no existe!");
	}
	return sbbBool[name];
}

// sbbVEC3

void SBB::postVEC3(string name, VEC3 value) {
	sbbVEC3[name] = value;
}

VEC3 SBB::readVEC3(string name) {
	if (sbbVEC3.find(name) == sbbVEC3.end())
	{
		fatal("sbbVEC3: Se intenta leer String que no existe!");
	}
	return sbbVEC3[name];
}

// sbbMoles
void SBB::postMole(string name, bt_mole* value) {
	sbbMole[name] = value;
}

bt_mole* SBB::readMole(string name) {
	if (sbbMole.find(name) == sbbMole.end())
	{
		fatal("sbbMole: Se intenta leer String que no existe!");
	}
	return sbbMole[name];
}

// sbbHandles

void SBB::postHandle(string name, CHandle value) {
	sbbHandle[name] = value;
}

CHandle SBB::readHandle(string name) {
	if (sbbHandle.find(name) == sbbHandle.end())
	{
		fatal("sbbHandle: Se intenta leer String que no existe!");
	}
	return sbbHandle[name];
}

// sbbHandlesVector

void SBB::postHandlesVector(string name, vector<CHandle> value) {
	sbbHandlesVector[name] = value;
}

vector<CHandle> SBB::readHandlesVector(string name) {
	if (sbbHandlesVector.find(name) == sbbHandlesVector.end())
	{
		fatal("sbbHandlesVector: Se intenta leer String que no existe!");
	}
	return sbbHandlesVector[name];
}

void SBB::postVEC3Vector(string name, vector<VEC3> value) {
	sbbVEC3Vector[name] = value;
}

vector<VEC3> SBB::readVEC3Vector(string name) {
	if (sbbVEC3Vector.find(name) == sbbVEC3Vector.end())
	{
		fatal("sbbVEC3Vector: Se intenta leer String que no existe!");
	}
	return sbbVEC3Vector[name];
}

void SBB::addVEC3ToVector(string name, VEC3 value) {
	if (sbbVEC3Vector.find(name) == sbbVEC3Vector.end())
	{
		fatal("sbbVEC3Vector: Se intenta leer String que no existe!");
	}
	sbbVEC3Vector[name].push_back(value);
}

void SBB::removeVEC3ToVector(string name, VEC3 value) {
	assert(false || fatal("Not implemented function!"));
}

// sbbGuardAlerts

void SBB::postGuardAlert(string name, guard_alert value) {
	sbbGuardAlerts[name] = value;
}

guard_alert SBB::readGuardAlert(string name) {
	if (sbbGuardAlerts.find(name) == sbbGuardAlerts.end())
	{
		fatal("sbbGuardAlerts: Se intenta leer String que no existe!");
	}
	return sbbGuardAlerts[name];
}

void SBB::updateGuardAlerts(float dt) {
	// update the timer of each alert
	for (std::map<string, guard_alert>::iterator alert_it = sbbGuardAlerts.begin(); alert_it != sbbGuardAlerts.end(); ) {
		alert_it->second.timer -= dt;

		if (alert_it->second.timer < 0.f) {
			alert_it = sbbGuardAlerts.erase(alert_it);
		}
		else {
			alert_it++;
		}
	}
}

vector<float> SBB::readLetterSpacingVector() {
	if (letterSpacing.size() == 0) {
		letterSpacing.resize(256, 0.0f);
		letterSpacing[65] = 0.0046875f;
		letterSpacing[66] = 0.0051136f;
		letterSpacing[67] = 0.00375f;
		letterSpacing[68] = 0.0046875f;
		letterSpacing[69] = 0.003515625f;
		letterSpacing[70] = 0.003515625f;
		letterSpacing[71] = 0.0046875f;
		letterSpacing[72] = 0.0040178f;
		letterSpacing[73] = 0.0033088f;
		letterSpacing[74] = 0.00375f;
		letterSpacing[75] = 0.0040178f;
		letterSpacing[76] = 0.003515625f;
		letterSpacing[77] = 0.0046875f;
		letterSpacing[78] = 0.0040178f;
		letterSpacing[79] = 0.005625f;
		letterSpacing[80] = 0.0040178f;
		letterSpacing[81] = 0.005625f;
		letterSpacing[82] = 0.0051136f;
		letterSpacing[83] = 0.0040178f;
		letterSpacing[84] = 0.00375f;
		letterSpacing[85] = 0.0040178f;
		letterSpacing[86] = 0.00375f;
		letterSpacing[87] = 0.00432692f;
		letterSpacing[88] = 0.00375f;
		letterSpacing[89] = 0.0040178f;
		letterSpacing[90] = 0.00375f;
		letterSpacing[97] = 0.0046875f;
		letterSpacing[98] = 0.0051136f;
		letterSpacing[99] = 0.00375f;
		letterSpacing[100] = 0.0046875f;
		letterSpacing[101] = 0.003515625f;
		letterSpacing[102] = 0.003515625f;
		letterSpacing[103] = 0.0046875f;
		letterSpacing[104] = 0.0040178f;
		letterSpacing[105] = 0.0033088f;
		letterSpacing[106] = 0.00375f;
		letterSpacing[107] = 0.0040178f;
		letterSpacing[108] = 0.003515625f;
		letterSpacing[109] = 0.0046875f;
		letterSpacing[110] = 0.0040178f;
		letterSpacing[111] = 0.005625f;
		letterSpacing[112] = 0.0040178f;
		letterSpacing[113] = 0.005625f;
		letterSpacing[114] = 0.0051136f;
		letterSpacing[115] = 0.0040178f;
		letterSpacing[116] = 0.00375f;
		letterSpacing[117] = 0.0040178f;
		letterSpacing[118] = 0.00375f;
		letterSpacing[119] = 0.00432692f;
		letterSpacing[120] = 0.00375f;
		letterSpacing[121] = 0.0040178f;
		letterSpacing[122] = 0.00375f;
	}
	return letterSpacing;
}