#include "mcv_platform.h"
#include "pila.h"

VHandles TCompPila::all_pilas;

void TCompPila::onCreate(const TMsgEntityCreated& msg)
{
	all_pilas.push_back(CHandle(this).getOwner());
}

void TCompPila::update(float elapsed) {
}

bool TCompPila::load(MKeyValue& atts) {
	return true;
}

TCompPila::~TCompPila() {
	removeFromVector(all_pilas, CHandle(this).getOwner());
}