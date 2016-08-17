#include "mcv_platform.h"
#include "comp_hierarchy.h"
#include "components/entity.h"

bool TCompHierarchy::load(MKeyValue& atts) {
	TCompTransform::load(atts);
	strcpy(parent_name, atts.getString("parent", "").c_str());
	h_parent_transform = CHandle();
	h_my_transform = CHandle();
	return true;
}

void TCompHierarchy::linkTo(CHandle h_entity) {
	CEntity* e = h_entity;
	if (!e) {
		h_parent_transform = CHandle();
		return;
	}
	h_parent_transform = e->get<TCompTransform>();
}

// Resolve parent
void TCompHierarchy::onGroupCreated(const TMsgEntityGroupCreated& msg) {
	CHandle h_entity = findByName(*msg.handles, parent_name);
	linkTo(h_entity);

	// Get access to my comp transform
	CHandle h_my_entity = CHandle(this).getOwner();
	CEntity* e_my_entity = h_my_entity;
	h_my_transform = e_my_entity->get<TCompTransform>();
}

void TCompHierarchy::updateWorldFromLocal() {
	const TCompTransform* t_parent = h_parent_transform;
	TCompTransform* t_local = h_my_transform;
	assert(t_local);
	t_local->combine(*t_parent, *this);
}