#include "mcv_platform.h"
#include "comp_culling.h"
#include "comp_aabb.h"
#include "comp_camera.h"
#include "comp_msgs.h"
#include "entity.h"

void TCompCulling::renderInMenu() {
	// Show how many AABB's we are seeing...
	auto hm = getHandleManager<TCompAbsAABB>();
	ImGui::Text("%d / %d AABB's visible", bits.count(), hm->size());
}

// Si algun plano tiene la caja en la parte negativa
// completamente, el aabb no esta dentro del set de planos
bool TCompCulling::VPlanes::isVisible(const AABB* aabb) const {
	auto it = begin();
	while (it != end()) {
		if (it->isCulled(aabb))
			return false;
		++it;
	}
	return true;
}

void TCompCulling::update() {
	// Get access to the comp_camera in a sibling component
	CEntity* e_owner = CHandle(this).getOwner();
	MAT44 view_proj;
	e_owner->sendMsg(TMsgGetCullingViewProj{ &view_proj });

	// Construir el set de planos usando la view_proj
	planes.fromViewProjection(view_proj);

	// Start from zero
	bits.reset();

	// Traverse all aabb's defined in the game
	// and test them
	auto hm = getHandleManager<TCompAbsAABB>();
	TCompAbsAABB* aabb = hm->getFirstObject();
	for (size_t i = 0; i < hm->size(); ++i, ++aabb) {
		if (planes.isVisible(aabb))
			bits.set(i);
	}
}