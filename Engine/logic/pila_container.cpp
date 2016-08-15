#include "mcv_platform.h"
#include "pila_container.h"

#include "components/entity.h"
#include "app_modules/logic_manager/logic_manager.h"

using namespace std;

VHandles TCompPilaContainer::all_pila_containers;

void TCompPilaContainer::onCreate(const TMsgEntityCreated& msg)
{
	all_pila_containers.push_back(ClHandle(this).getOwner());
}

void TCompPilaContainer::PutPila(ClHandle new_pila, bool notify) {
	pila = new_pila;

	//Logic Event
	//char params[256];
	//TMsgIsCharged msg;
	//pila.sendMsgWithReply(msg);
	//string state = msg.charged ? "charged" : "uncharged";
	//sprintf(params, "%s,%s", e->getName(), state);
	if (notify) {
		CEntity* e = ClHandle(this).getOwner();
		logic_manager->throwEvent(CLogicManagerModule::OnPutPila, e->getName(), pila);
	}
}

void TCompPilaContainer::RemovePila() {
	ClHandle copyPilaforLM = pila;
	pila = ClHandle();

	//Logic Event
	CEntity* e = ClHandle(this).getOwner();
	logic_manager->throwEvent(CLogicManagerModule::OnRemovePila, e->getName(), copyPilaforLM);
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
	removeFromVector(all_pila_containers, ClHandle(this).getOwner());
}