#ifndef INC_BOX_COMPONENT_H_
#define	INC_BOX_COMPONENT_H_

#include "comp_base.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_physics.h"

//----------------------------
#include "logic\sbb.h"

struct TCompBox : public TCompBase {
	bool moving						= false;
	bool carePosition				= true;
	bool added						= false;

	VEC3 originPoint;
	float dist_separation	= 5.0f;

	CHandle mParent;

	

	void init() {
		mParent = CHandle(this).getOwner();
		if (carePosition) {
			CEntity *e = mParent;
			TCompTransform *t = e->get<TCompTransform>();
			originPoint = t->getPosition();
		}
	}

	void update(float elapsed) {
		if (carePosition && !added) {
			if (!mParent.isValid()) return;
			CEntity *e = mParent;
			TCompTransform *t = e->get<TCompTransform>();
			float d = simpleDist(t->getPosition(),originPoint);
			if (d > dist_separation) {
				ImTooFar();
			}

		}
	}

	void ImTooFar() {
		VHandles hs;
		hs = SBB::readHandlesVector("wptsBoxes");
		hs.push_back(mParent);
		SBB::postHandlesVector("wptsBoxes", hs);
		added = !added;
	}

	bool load(MKeyValue& atts) {
		carePosition = atts.getBool("fixed",false);
		return true;
	}

	VEC3 GetLeavePoint() const { return originPoint; }

	void onUnLeaveBox(const TMsgLeaveBox& msg) {
		added = !added;
		VHandles hs;
		hs = SBB::readHandlesVector("wptsBoxes");
		hs.erase(std::remove(hs.begin(), hs.end(), mParent), hs.end());
		SBB::postHandlesVector("wptsBoxes", hs);
		CEntity *e = mParent;
		if (!e) return;
		TCompPhysics *p = e->get<TCompPhysics>();
		TCompTransform *t = e->get<TCompTransform>();
		originPoint = t->getPosition();
		p->setKinematic(false);
	}

};

#endif