#include "mcv_platform.h"
#include <windows.h>
#include "sbb.h"
#include "handle\handle.h"

map<string, int> SBB::sbbInt;
map<string, bool> SBB::sbbBool;
map<string, VEC3> SBB::sbbVEC3;
map<string, vector<CHandle>> SBB::sbbHandles;

void SBB::init() {
}

void SBB::postInt(string name, int value) {
	sbbInt[name] = value;
}

int SBB::readInt(string name) {
	if (sbbInt.find(name) == sbbInt.end())
	{
		fatal("SBB: Se intenta leer String que no existe!");
	}
	return sbbInt[name];
}

void SBB::inc(string name) {
	if (sbbInt.find(name) == sbbInt.end())
	{
		fatal("SBB: Se intenta incrementar String que no existe!");
	}
	sbbInt[name]++;
}

void SBB::dec(string name) {
	if (sbbInt.find(name) == sbbInt.end())
	{
		fatal("SBB: Se intenta decrementar String que no existe!");
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
		fatal("SBB: Se intenta leer String que no existe!");
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
		fatal("SBB: Se intenta leer String que no existe!");
	}
	return sbbVEC3[name];
}

// sbbHandles

void SBB::postHandles(string name, vector<CHandle> value) {
	sbbHandles[name] = value;
}

vector<CHandle> SBB::readHandles(string name) {
	if (sbbHandles.find(name) == sbbHandles.end())
	{
		fatal("SBB: Se intenta leer String que no existe!");
	}
	return sbbHandles[name];
}
