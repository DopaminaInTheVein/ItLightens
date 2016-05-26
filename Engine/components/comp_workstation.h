#ifndef INC_WORKSTATION_COMPONENT_H_
#define	INC_WORKSTATION_COMPONENT_H_

#include "comp_base.h"
#include "entity.h"
#include "comp_transform.h"

struct TCompWorkstation : public TCompBase {

	CHandle mParent;	
	VEC3 position;

	void init() {
		mParent = CHandle(this).getOwner();		
		CEntity *e = mParent;
		TCompTransform *t = e->get<TCompTransform>();
		position = t->getPosition();
	}

	void update(float elapsed) {
		// nothing to be done
	}

	VEC3 getPosition() {
		return position;
	}
};

#endif