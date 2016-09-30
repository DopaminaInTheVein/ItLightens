#include "mcv_platform.h"
#include "comp_hierarchy.h"
#include "components/entity.h"
#include "entity_parser.h"

bool TCompHierarchy::load(MKeyValue& atts) {

	TCompTransform::load(atts);
	parent_id = atts.getInt("parent", 0);
	h_parent_transform = CHandle();
	h_my_transform = CHandle();
	return true;
}

void TCompHierarchy::onCreate(const TMsgEntityCreated&)
{
	if (myHandle.isValid()) myHandle.destroy();
	myHandle = CHandle(this).getOwner();
	IdEntities::addHierarchyHandle(myHandle);
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
void TCompHierarchy::onGetParentById(const TMsgHierarchySolver&) {
	CHandle h_entity = IdEntities::findById(parent_id);
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