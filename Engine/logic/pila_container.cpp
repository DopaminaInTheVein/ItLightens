#include "mcv_platform.h"
#include "pila_container.h"

VHandles TCompPilaContainer::all_pila_containers;

void TCompPilaContainer::onCreate(const TMsgEntityCreated& msg)
{
	all_pila_containers.push_back(CHandle(this).getOwner());
}

void TCompPilaContainer::PutPila(CHandle new_pila) {
	pila = new_pila;
}

void TCompPilaContainer::RemovePila() {
	pila = CHandle();
}

bool TCompPilaContainer::HasPila() {
	return pila.isValid();
}

TCompPilaContainer::~TCompPilaContainer() {
	removeFromVector(all_pila_containers, CHandle(this).getOwner());
}