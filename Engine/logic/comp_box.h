#ifndef INC_BOX_COMPONENT_H_
#define	INC_BOX_COMPONENT_H_

#include "components/comp_base.h"
#include "handle/handle.h"

struct TCompBox : public TCompBase {
	static VHandles all_boxes;
	bool moving = false;
	bool carePosition = true;
	bool removable = true;
	bool added = false;

	CHandle mParent;
	VEC3 originPoint;
	float dist_separation = 5.0f;

	~TCompBox();
	void init();
	void update(float elapsed);
	bool load(MKeyValue& atts);

	void stuntNpcs();
	void ImTooFar();
	VEC3 GetLeavePoint() const;
	bool isRemovable();
	void onUnLeaveBox(const TMsgLeaveBox& msg);
};

#endif