#ifndef INC_LOGIC_PILA_CONTAINER_H_
#define	INC_LOGIC_PILA_CONTAINER_H_

#include "components/comp_base.h"

struct TCompPilaContainer : public TCompBase {
	static VHandles all_pila_containers;
	CHandle player;
	CHandle pila;

	~TCompPilaContainer();
	void onCreate(const TMsgEntityCreated& msg);
	void PutPila(CHandle);
	void RemovePila();
	bool HasPila();
};

#endif