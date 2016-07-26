#include "mcv_platform.h"
#include "comp_culling.h"
#include "comp_aabb.h"
#include "comp_camera.h"
#include "comp_msgs.h"
#include "entity.h"
#include "comp_room.h"
#include "logic\sbb.h"

void TCompCulling::renderInMenu() {
	// Show how many AABB's we are seeing...
	auto hm = getHandleManager<TCompAbsAABB>();
	ImGui::Text("%d / %d AABB's visible", bits.count(), hm->size());
}

// Si algun plano tiene la caja en la parte negativa
// completamente, el aabb no esta dentro del set de planos
bool TCompCulling::VPlanes::isVisible(const AABB* aabb) const {
	PROFILE_FUNCTION("TCompCulling: isVisible func");
	auto it = begin();
	while (it != end()) {
		if (it->isCulled(aabb))
			return false;
		++it;
	}
	return true;
}

void TCompCulling::update() {
	//PROFILE_FUNCTION("TCompCulling: Update");
	// Get access to the comp_camera in a sibling component

	CEntity* e_owner = CHandle(this).getOwner();
	if (!e_owner) return;
	TCompRoom* room = e_owner->get<TCompRoom>();
	if (room) {
		std::vector<int> rooms = room->name;
		if (std::find(rooms.begin(), rooms.end(), SBB::readSala()) == rooms.end()) {
			return;			//light on diferent room
		}
		else {
			//fast fix for room3
			if (SBB::readSala() == 2) {
				CEntity* ep = tags_manager.getFirstHavingTag("player");
				if (ep) {
					TCompTransform* t = ep->get<TCompTransform>();
					TCompTransform* tl = e_owner->get<TCompTransform>();

					if (t->getPosition().y > 10) {
						if (tl->getPosition().y < 12)
							return;
					}
					else {
						if (tl->getPosition().y > 12)
							return;
					}
				}
			}
		}
	}

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
		PROFILE_FUNCTION("TCompCulling: isVisible bucle");
		if (planes.isVisible(aabb))
			bits.set(i);
	}
}