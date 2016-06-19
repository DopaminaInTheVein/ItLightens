#include "mcv_platform.h"
#include <windows.h>
#include <string>
#include "sbb.h"
#include "handle/handle.h"
#include "logic/bt_mole.h"
#include "logic/bt_guard.h"
#include "recast/navmesh.h"

string SBB::sbbSala;
CNavmesh SBB::sbbNav;
map<string, int> SBB::sbbInt;
map<string, bool> SBB::sbbBool;
map<string, VEC3> SBB::sbbVEC3;
map<string, bt_mole*> SBB::sbbMole;
map<string, CHandle> SBB::sbbHandle;
map<string, vector<CHandle>> SBB::sbbHandlesVector;
map<string, vector<VEC3> > SBB::sbbVEC3Vector;
map<string, guard_alert> SBB::sbbGuardAlerts;

void SBB::init() {
	postBool("possMode", false);
	postVEC3Vector("wb_pos", vector<VEC3>());
}

void SBB::update(float dt) {
	updateGuardAlerts(dt);
}

void SBB::postSala(string sala) {
	sbbSala = sala;
}
string SBB::readSala() {
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