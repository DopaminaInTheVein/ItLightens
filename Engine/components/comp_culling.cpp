#include "mcv_platform.h"
#include "comp_culling.h"
#include "comp_aabb.h"
#include "comp_camera.h"
#include "comp_msgs.h"
#include "entity.h"
#include "handle/handle.h"
#include "comp_room.h"
#include "logic\sbb.h"
#include "player_controllers/player_controller_base.h"

int TCompCulling::next_to_update = 0;
CHandle TCompCulling::camera_main = CHandle();
bool TCompCulling::cull_camera = true;

void TCompCulling::renderInMenu() {
	// Show how many AABB's we are seeing...
	auto hm = getHandleManager<TCompAbsAABB>();
	ImGui::Text("%d / %d AABB's visible", bits.count(), hm->size());
}

// Si algun plano tiene la caja en la parte negativa
// completamente, el aabb no esta dentro del set de planos
bool TCompCulling::VPlanes::isVisible(const AABB* aabb) const {
	PROFILE_FUNCTION("TCompCulling: isVisible func");
	if (empty()) return true;
	auto it = begin();
	while (it != end()) {
		if (it->isCulled(aabb))
			return false;
		++it;
	}
	return true;
}

// Returns true if the aabb is fully in the negative side of the plane
bool TCompCulling::CPlane::isCulled(const AABB* aabb) const {
	//
	const float r = aabb->Extents.x * fabsf(n.x)
		+ aabb->Extents.y * fabsf(n.y)
		+ aabb->Extents.z * fabsf(n.z)
		;

	// Distance from box center to the plane
	const float c = n.Dot(aabb->Center) + d;
	return c < -r;
}

void TCompCulling::update() {
	//PROFILE_FUNCTION("TCompCulling: Update");
	// Get access to the comp_camera in a sibling component
	compBaseEntity = MY_OWNER;
	if (!compBaseEntity) return;
	GET_MY(room, TCompRoom);
	if (room && !room->sameRoomPlayer()) return;

	MAT44 view_proj;
	MY_OWNER.sendMsg(TMsgGetCullingViewProj{ &view_proj });

	// Construir el set de planos usando la view_proj
	planes.fromViewProjection(view_proj);

	// Start from zero
	bits.reset();

	// Traverse all aabb's defined in the game
	// and test them
	auto hm = getHandleManager<TCompAbsAABB>();
	TCompAbsAABB* aabb = hm->getFirstObject();
	PROFILE_FUNCTION("TCompCulling: do culling");
	for (size_t i = 0; i < hm->size(); ++i, ++aabb) {
		PROFILE_FUNCTION("TCompCulling: do culling each");

		CHandle aabb_h = aabb;
		CHandle aabb_h_owner = aabb_h.getOwner();

		bool isPlayer = aabb_h_owner == CPlayerBase::handle_player;
		/*{
			PROFILE_FUNCTION("TCompCulling: isPlayer");
			isPlayer = MY_OWNER == CPlayerBase::handle_player;
		}*/
		{
			PROFILE_FUNCTION("TCompCulling: isVisible");
			if (isPlayer) {
				bits.set(i);
			}
			else if (planes.isVisible(aabb)) {
				bits.set(i);
			}
		}
	}
}

bool TCompCulling::checkAABB(TCompCulling* culling, TCompAbsAABB* aabb)
{
	TCompCulling * cculling = culling;
	TCompCulling::TCullingBits* culling_bits = nullptr;
	culling_bits = &cculling->bits;

	// To get the index of each aabb
	auto hm_aabbs = getHandleManager<TCompAbsAABB>();
	const TCompAbsAABB* base_aabbs = hm_aabbs->getFirstObject();

	intptr_t idx = aabb - base_aabbs;
	return culling_bits->test(idx);
}

void TCompCulling::updateNext()
{
	if (cull_camera) {
		if (!camera_main.isValid()) camera_main = tags_manager.getFirstHavingTag("camera_main");
		if (camera_main.isValid()) {
			GET_COMP(cul_cam, camera_main, TCompCulling);
			if (cul_cam) cul_cam->update();
		}
	}
	else {
		TCompCulling* cullings = getHandleManager<TCompCulling>()->getFirstObject();
		(cullings + next_to_update++)->update();
		if (next_to_update > getHandleManager<TCompCulling>()->size()) TCompCulling::next_to_update = 0;
	}
	cull_camera = !cull_camera;
}