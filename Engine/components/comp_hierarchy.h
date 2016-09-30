#ifndef INC_COMPONENT_HIERARCHY_H_
#define INC_COMPONENT_HIERARCHY_H_

#include "comp_transform.h"

struct TCompHierarchy : public TCompTransform {
	CHandle myHandle;
	CHandle h_parent_transform;
	CHandle h_my_transform;
	int  parent_id;
	void linkTo(CHandle e);
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
	void onGetParentById(const TMsgHierarchySolver&);
	void updateWorldFromLocal();
};

#endif
