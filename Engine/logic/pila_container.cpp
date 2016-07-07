#include "mcv_platform.h"
#include "pila_container.h"

#include "components/entity.h"
#include "app_modules/logic_manager/logic_manager.h"

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

bool TCompPilaContainer::HasPilaCharged() {
	if (pila.isValid()) {
		TMsgIsCharged msg;
		pila.sendMsgWithReply(msg);
		return msg.charged;
	}
	return false;
}

void TCompPilaContainer::onRecharge(const TMsgSetCharged& msg) {
	if (pila.isValid()) {
		assert(msg.charged || fatal("Cannot empty the cell! (not supported)\n"));
		if (msg.charged) pila.sendMsg(TMsgActivate());
	}
}

TCompPilaContainer::~TCompPilaContainer() {
	removeFromVector(all_pila_containers, CHandle(this).getOwner());
}