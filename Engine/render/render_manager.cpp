#include "mcv_platform.h"
#include "render_manager.h"
#include "static_mesh.h"
#include "material.h"
#include "technique.h"
#include "mesh.h"
#include "components/comp_transform.h"
#include "components/comp_aabb.h"
#include "components/comp_culling.h"
#include "skeleton/comp_skeleton.h"
#include "components/entity.h"
#include "components/comp_room.h"
#include "logic/sbb.h"
#include "render/draw_utils.h"

CRenderManager RenderManager;

#include "render/shader_cte.h"

//polarity
#include "player_controllers\player_controller.h"	//for player
#include "components\comp_polarized.h"				//for polarized objects

// -------------------------------------------------
// We need to implement both operators
bool operator<(const CRenderManager::TKey& k, const CRenderTechnique::eCategory cat) {
	return k.material->tech->getCategory() < cat;
}

bool operator<(const CRenderTechnique::eCategory cat, const CRenderManager::TKey& k) {
	return cat < k.material->tech->getCategory();
}

// -------------------------------------------------
bool CRenderManager::sortByTechMatMesh(
	const TKey &k1
	, const TKey &k2) {
	auto* tech1 = k1.material->tech;
	auto* tech2 = k2.material->tech;

	if (tech1 != tech2) {
		if (tech1->getCategory() != tech2->getCategory())
			return tech1->getCategory() < tech2->getCategory();
		if (tech1->getCategory() != tech2->getCategory())
			return tech1->getCategory() < tech2->getCategory();
		if (tech1->getPriority(k1.owner.getOwner()) == tech2->getPriority(k2.owner.getOwner()))
			return tech1->getName() < tech2->getName();

		float prio1 = tech1->getPriority(k1.owner.getOwner());
		float prio2 = tech2->getPriority(k2.owner.getOwner());
		return (prio1 < prio2);
	}

	//ui are compared by z position
	else if (tech1->getCategory() == CRenderTechnique::UI_OBJS && tech2->getCategory() == CRenderTechnique::UI_OBJS) {
		if (tech1->getPriority(k1.owner.getOwner()) == tech2->getPriority(k2.owner.getOwner()))
			return tech1->getName() < tech2->getName();

		float prio1 = tech1->getPriority(k1.owner.getOwner());
		float prio2 = tech2->getPriority(k2.owner.getOwner());
		return (prio1 < prio2);
	}

	return k1.material < k2.material;
}

void CRenderManager::registerToRender(const CStaticMesh* mesh, CHandle owner) {
	CHandle ow = owner.getOwner();
	CEntity* e = ow;
	assert(e);
	CHandle h_transform = e->get<TCompTransform>();
	CHandle h_aabb = e->get<TCompAbsAABB>();
	TCompRoom * comproom = e->get<TCompRoom>();
	std::vector<int> oroom;
	oroom.push_back(-1);
	if (comproom) {
		oroom = comproom->name;
	}
	bool playercandidate = ow.hasTag("player") || ow.hasTag("AI_poss");
	for (auto s : mesh->slots) {
		TKey k;
		k.material = s.material;
		k.mesh = s.mesh;
		k.owner = owner;
		k.transform = h_transform;
		k.submesh_idx = s.submesh_idx;
		k.aabb = h_aabb;
		k.room = oroom;
		k.isPlayer = playercandidate;
		all_keys.push_back(k);
	}

	// Comprobar si juntando todos los slots de mesh
	// tengo la mesh completa...
	// De momento, voy a asumir que si....
	for (auto s : mesh->slots) {
		if (s.generates_shadows) {
			TShadowKey k;
			k.mesh = s.mesh;
			k.owner = owner;
			k.transform = h_transform;
			k.room = oroom;
			k.aabb = h_aabb;
			k.isPlayer = playercandidate;
			if (!s.material->tech->usesBones())
				all_shadow_keys.push_back(k);
			else
				all_shadow_skinning_keys.push_back(k);
		}
	}

	in_order = false;
}

void sortUI() {
}

void CRenderManager::unregisterFromRender(CHandle owner) {
	// Pasarse por todas las keys y borrar aquellas q tengan el owner
	auto it = all_keys.begin();
	while (it != all_keys.end()) {
		if (it->owner == owner) {
			it = all_keys.erase(it);
		}
		else
			++it;
	}
}

static bool ui_render = false;
static int id = 0;

void checkTestZ(std::vector<std::string>& last_v, std::vector<std::string>& new_v) {
	bool same = true;
	if (last_v.empty()) {
		same = false;
		for (auto it : new_v) {
			last_v.push_back(it);
		}
	}
	else {
		for (int idx = 0; idx < last_v.size(); idx++) {
			//cheack is some it are different
			std::string str1 = last_v[idx];
			std::string str2 = idx >= new_v.size() ? "" : new_v[idx];

			if (strcmp(str1.c_str(), str2.c_str())) {
				same = false;

				//rebuild last_v
				last_v.clear();
				for (auto it : new_v) {
					last_v.push_back(it);
				}
				break;
			}
		}
	}

	//print if different
	if (!same) {
		id++;
		dbg("#### INIT %d #####\n", id);
		for (auto it : last_v) {
			dbg("\nz_test_ui : %s", it.c_str());
		}
		dbg("#### END %d #####\n", id);
	}
}

#include "components\comp_name.h"

void CRenderManager::renderAll(CHandle h_camera, CRenderTechnique::eCategory category) {
	std::string name;
	ui_render = false;
	test_z_render.clear();

	PROFILE_FUNCTION("OBJS");
	if (category == CRenderTechnique::DBG_OBJS) {
		name = "DEBUG_OBJS";
		PROFILE_FUNCTION("DEBUG_OBJS");
	}
	else if (category == CRenderTechnique::SOLID_OBJS) {
		name = "SOLID_OBJS";
		PROFILE_FUNCTION("SOLID_OBJS");
	}
	else if (category == CRenderTechnique::TRANSPARENT_OBJS) {
		name = "TRANSPARENT_OBJS";
		PROFILE_FUNCTION("TRANSPARENT_OBJS");
	}
	else if (category == CRenderTechnique::UI_OBJS) {
		name = "UI_OBJS";
		PROFILE_FUNCTION("UI_OBJS");
		ui_render = true;
	}
	else if (category == CRenderTechnique::DETAIL_OBJS) {
		name = "DETAIL_OBJS";
		PROFILE_FUNCTION("DETAIL_OBJS");
	}
	else {
		name = "UNKNOWN_OBJS";
		PROFILE_FUNCTION("UNKNOWN_OBJS");
	}
	CTraceScoped scope(name.c_str());

	if (!in_order) {
		// sort the keys based on....

		std::sort(all_keys.begin(), all_keys.end(), &sortByTechMatMesh);
		in_order = true;
		++ntimes_sorted;
	}

	if (all_keys.empty())
		return;

	// Check if we have culling information from the camera source
	CEntity* e_camera = h_camera;
	TCompCulling::TCullingBits* culling_bits = nullptr;
	TCompCulling* culling = nullptr;
	if (e_camera)
		culling = e_camera->get<TCompCulling>();
	if (culling)
		culling_bits = &culling->bits;
	// To get the index of each aabb
	auto hm_aabbs = getHandleManager<TCompAbsAABB>();
	const TCompAbsAABB* base_aabbs = hm_aabbs->getFirstObject();

	//
	auto r = std::equal_range(
		all_keys.begin()
		, all_keys.end()
		, category
		);
	auto d0 = std::distance(all_keys.begin(), r.first);
	auto d1 = std::distance(all_keys.begin(), r.second);
	TKey* it = &all_keys[0] + d0;
	TKey* end_it = &all_keys[0] + d1;

	static TKey null_key;
	memset(&null_key, 0x00, sizeof(TKey));
	const TKey* prev_it = &null_key;

	bool curr_tech_used_bones = false;
	int  nkeys_rendered = 0;
	int pj_room = SBB::readSala();
	// Pasearse por todas las keys
	while (it != end_it) {
		// Do the culling
		if (it->owner.getOwner().hasTag("tasklist") || it->owner.getOwner().hasTag("tasklistend")) {
			CEntity * tent = it->owner.getOwner();
			TCompRoom * tentroom = tent->get<TCompRoom>();
			it->room = tentroom->name;
		}

		if (it->owner.getOwner().hasTag("player") || pj_room == -1 || it->room[0] == -1 || std::find(it->room.begin(), it->room.end(), pj_room) != it->room.end()) {
			if (culling_bits) {
				TCompAbsAABB* aabb = it->aabb;
				if (aabb) {
					intptr_t idx = aabb - base_aabbs;
					if (!culling_bits->test(idx)) {
						++it;
						continue;
					}
				}
			}

			if (it->isPlayer) {
				CEntity* e_p = it->owner.getOwner();
				if (e_p) {
					player_controller* pc = e_p->get<player_controller>();
					if (pc) {
						it->polarity = pc->GetPolarityInt();
					}
					else
						it->polarity = 0;
				}
				else {
					it->polarity = 0;
				}
			}
			else {
				CEntity* e_o = it->owner.getOwner();
				if (e_o) {
					TCompPolarized* op = e_o->get<TCompPolarized>();
					if (op) {
						it->polarity = op->force.polarity;
					}
					else
						it->polarity = 0;
				}
				else {
					it->polarity = 0;
				}
			}

			if (it->material != prev_it->material) {
				if (!prev_it->material || it->material->tech != prev_it->material->tech) {
					it->material->tech->activate();
					curr_tech_used_bones = it->material->tech->usesBones();
				}
				it->material->activateTextures(it->polarity);
			}
			else {
				if (it->polarity != prev_it->polarity) {
					it->material->activateTextures(it->polarity);
				}
			}
			if (it->mesh != prev_it->mesh)
				it->mesh->activate();
			if (it->owner != prev_it->owner) {
				// subir la world de it
				const TCompTransform* c_tmx = it->transform;
				assert(c_tmx);

				// For static objects, we could skip this step
				// if each static object had it's own shader_ctes_object
				activateWorldMatrix(c_tmx->asMatrix());
			}

			//render skeleton object
			if (curr_tech_used_bones) {
				if (renderSkeleton(it)) {
					//valid skeleton
					it->mesh->renderGroup(it->submesh_idx);
					prev_it = it;
					++nkeys_rendered;
				}
			}

			//render UI object
			else if (it->material->tech->getCategory() == CRenderTechnique::UI_OBJS) {
				if (renderUI(it)) {
					//Valid UI
					it->mesh->renderGroup(it->submesh_idx);
					prev_it = it;
					++nkeys_rendered;

					CEntity* e_owner = it->owner.getOwner();
					TCompName* e_name = e_owner->get<TCompName>();
					test_z_render.push_back(e_name->name);
				}
			}
			else {
				//default skeleton
				it->mesh->renderGroup(it->submesh_idx);
				prev_it = it;
				++nkeys_rendered;
			}
		}

		++it;
	}

#ifndef NDEBUG
	if (ui_render)checkTestZ(last_test_z_render, test_z_render);
#endif

	CMaterial::deactivateTextures();

	renderedCulling.push_back(nkeys_rendered);
}

bool CRenderManager::renderSkeleton(TKey* it) {
	const CEntity* e = it->owner.getOwner();
	if (!e) return false;
	const TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
	if (!comp_skel) return false;
	comp_skel->uploadBonesToCteShader();
	return true;
}

#include "app_modules\gui\comps\gui_basic.h"
bool CRenderManager::renderUI(TKey* it) {
	const CEntity* e = it->owner.getOwner();
	if (!e) return false;
	TCompGui* comp_ui = e->get<TCompGui>();
	if (comp_ui) {
		shader_ctes_gui.state_ui = comp_ui->getRenderState();
		comp_ui->uploadCtes();
	}
	else {
		shader_ctes_gui.state_ui = 0;
	}

	shader_ctes_gui.uploadToGPU();

	return true;
}

void CRenderManager::renderUICulling() {
	for (int i = 0; i < renderedCulling.size(); i++) {
		ImGui::Text("%d/%ld keys of call %d", renderedCulling[i], all_keys.size(), i);
	}

	renderedCulling.clear();
}

// ------------------------------------------
void CRenderManager::renderShadowCasters(CHandle h_light) {
	CTraceScoped scope("Shadow Casters");
	PROFILE_FUNCTION("SHADOW CASTERS OBJ");
	auto it = all_shadow_keys.begin();
	int pj_room = SBB::readSala();

	// Check if we have culling information from the camera source
	CEntity* e_camera = h_light;
	TCompCulling::TCullingBits* culling_bits = nullptr;
	TCompCulling* culling = nullptr;

	TCompRoom* room;

	if (e_camera) {
		culling = e_camera->get<TCompCulling>();
		room = e_camera->get<TCompRoom>();
	}
	else {
		return;
	}

	//check room
	/*std::vector<int> room_str;
	room_str.push_back(-1);
	if (room) {
		room_str = room->name;
	}
	*/
	//culling
	if (culling)
		culling_bits = &culling->bits;
	// To get the index of each aabb
	auto hm_aabbs = getHandleManager<TCompAbsAABB>();
	const TCompAbsAABB* base_aabbs = hm_aabbs->getFirstObject();
	int nkeys_rendered = 0;
	while (it != all_shadow_keys.end()) {
		if (pj_room == -1 || it->room[0] == -1 || std::find(it->room.begin(), it->room.end(), pj_room) != it->room.end()) {
			if (culling_bits) {
				TCompAbsAABB* aabb = it->aabb;
				if (aabb) {
					intptr_t idx = aabb - base_aabbs;
					if (!culling_bits->test(idx)) {
						++it;
						continue;
					}
					nkeys_rendered++;
				}
			}

			const TCompTransform* c_tmx = it->transform;
			//TODO: Review Pedro!
			//assert(c_tmx);
			if (c_tmx) {
				activateWorldMatrix(c_tmx->asMatrix());
				// If the shadows_keys were sorted by mesh
				// I could skip the activation and just call it
				// when the mesh changed, and only call the render
				it->mesh->activateAndRender();
			}
			else {
				//Puede no tener transform, ignoralo y ya esta
				//fatal("render__manager: tranfrom from shadowcaster null");
			}
		}

		++it;
	}

	renderedCulling.push_back(nkeys_rendered);
}

// ------------------------------------------

//render shadowcaster with skinning tech
void CRenderManager::renderShadowCastersSkin(CHandle h_light) {
	PROFILE_FUNCTION("SHADOW CASTER SKIN");
	auto it = all_shadow_skinning_keys.begin();
	int pj_room = SBB::readSala();

	CEntity* e_camera = h_light;
	TCompRoom* room;

	if (e_camera) {
		room = e_camera->get<TCompRoom>();
	}
	else
		return;

	//check room
	std::vector<int> room_str;
	room_str.push_back(-1);
	if (room) {
		room_str = room->name;
	}

	if (std::find(room_str.begin(), room_str.end(), SBB::readSala()) == room_str.end())
		return;			//shadows on diferent room
	else {
		//fast fix for room3
		if (SBB::readSala() == 2) {
			CEntity* ep = tags_manager.getFirstHavingTag("player");
			if (ep) {
				TCompTransform* t = ep->get<TCompTransform>();
				TCompTransform* tl = e_camera->get<TCompTransform>();

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

	while (it != all_shadow_skinning_keys.end()) {
		if (it->owner.getOwner().hasTag("player") || pj_room == -1 || it->room[0] == -1 || std::find(it->room.begin(), it->room.end(), pj_room) != it->room.end()) {
			const TCompTransform* c_tmx = it->transform;

			if (c_tmx) {
				activateWorldMatrix(c_tmx->asMatrix());

				const CEntity* e = it->owner.getOwner();
				assert(e);
				const TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
				assert(comp_skel);
				comp_skel->uploadBonesToCteShader();

				// If the shadows_keys were sorted by mesh
				// I could skip the activation and just call it
				// when the mesh changed, and only call the render
				it->mesh->activateAndRender();
			}
			else {
				//fatal("render__manager: tranfrom from shadowcaster skinning null");
			}
		}
		++it;
	}
}