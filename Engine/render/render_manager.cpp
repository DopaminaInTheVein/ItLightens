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
#include "player_controllers/player_controller_base.h"
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

bool operator<(const CRenderTechnique::eCategory cat, const CRenderManager::TShadowKey& k) {
	return 0;
}

bool operator<(const CRenderManager::TShadowKey& k, const CRenderTechnique::eCategory cat) {
	return 0;
}

// -------------------------------------------------
bool CRenderManager::sortByTechMatMesh(
	const TKey &k1
	, const TKey &k2) {
	auto* tech1 = k1.material->tech;
	auto* tech2 = k2.material->tech;

	PROFILE_FUNCTION("Render manager: sort");

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

bool CRenderManager::sortByMesh(
	const TShadowKey &k1
	, const TShadowKey &k2) {


	PROFILE_FUNCTION("Render manager: sort");

	//shadow caster are sorted by mesh
	return k1.mesh < k2.mesh;
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
		for (auto room : oroom) {
			if (room >= 0 && !playercandidate)
				all_keys[room].push_back(k);
			else {
				for (int idx = 0; idx < ROOMS_SIZE; idx++) {
					all_keys[idx].push_back(k);
				}
			}
		}
		num_renders++;
		in_order = false;
	}

	// Comprobar si juntando todos los slots de mesh
	// tengo la mesh completa...
	// De momento, voy a asumir que si....
	for (auto s : mesh->slots) {

		//only solid objects cast shadows
		if (s.material->tech->getCategory() != CRenderTechnique::SOLID_OBJS)
			continue;

		if (s.generates_shadows) {
			TShadowKey k;
			k.mesh = s.mesh;
			k.owner = owner;
			k.transform = h_transform;
			k.room = oroom;
			k.aabb = h_aabb;
			k.isPlayer = playercandidate;
			if (!s.material->tech->usesBones()) {
				for (auto room : oroom) {
					if (room >= 0 && !playercandidate)
						all_shadow_keys[room].push_back(k);
					else {
						for (int idx = 0; idx < ROOMS_SIZE; idx++) {
							all_shadow_keys[idx].push_back(k);
						}
					}
				}
				in_order_shadows = false;
			}
			else {
				for (auto room : oroom) {
					if (room >= 0 && !playercandidate)
						all_shadow_skinning_keys[room].push_back(k);
					else {
						for (int idx = 0; idx < ROOMS_SIZE; idx++) {
							all_shadow_skinning_keys[idx].push_back(k);
						}
					}
				}
				in_order_shadows_skin = false;
			}
		}
	}

	
	
}


void CRenderManager::unregisterFromRender(CHandle owner) {

	// Pasarse por todas las keys y borrar aquellas q tengan el owner

	for (int idx = 0; idx < ROOMS_SIZE; idx++) {
		auto it = all_keys[idx].begin();
		while (it != all_keys[idx].end()) {
			if (it->owner == owner) {
				num_renders--;
				it = all_keys[idx].erase(it);
			}
			else
				++it;
		}
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

void CRenderManager::renderAll(CHandle h_camera, CRenderTechnique::eCategory category, int room) {
	std::string name;
	ui_render = false;
	//test_z_render.clear();

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

		for (int idx = 0; idx < ROOMS_SIZE; idx++) {
			std::sort(all_keys[idx].begin(), all_keys[idx].end(), &sortByTechMatMesh);
		}
		in_order = true;
		++ntimes_sorted;
	}

	if (all_keys[room].empty())
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
		all_keys[room].begin()
		, all_keys[room].end()
		, category
	);
	auto d0 = std::distance(all_keys[room].begin(), r.first);
	auto d1 = std::distance(all_keys[room].begin(), r.second);
	TKey* it = &all_keys[room][0] + d0;
	TKey* end_it = &all_keys[room][0] + d1;

	static TKey null_key;
	memset(&null_key, 0x00, sizeof(TKey));
	const TKey* prev_it = &null_key;

	bool curr_tech_used_bones = false;
	int  nkeys_rendered = 0;
	int pj_room = SBB::readSala();
	// Pasearse por todas las keys
	while (it != end_it) {
		PROFILE_FUNCTION("Render Manager each");
		// Do the culling
		{
			GET_COMP(tentroom, it->owner.getOwner(), TCompRoom);
			if (tentroom) it->room = tentroom->name;
			//PROFILE_FUNCTION("Render Manager: Check Tasklist tag");
			//if (it->owner.getOwner().hasTag("tasklist") || it->owner.getOwner().hasTag("tasklistend")) {
			//	CEntity * tent = it->owner.getOwner();
			//	TCompRoom * tentroom = tent->get<TCompRoom>();
			//	it->room = tentroom->name;
			//}
		}
		if (it->owner.getOwner() == CPlayerBase::handle_player || pj_room == -1 || it->room[0] == -1 || std::find(it->room.begin(), it->room.end(), pj_room) != it->room.end()) {
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
	//if (ui_render)checkTestZ(last_test_z_render, test_z_render);
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

void CRenderManager::renderUICulling(int room) {
	for (int i = 0; i < renderedCulling.size(); i++) {
		ImGui::Text("%d/%ld keys of call %d", renderedCulling[i], all_keys[room].size(), i);
	}

	renderedCulling.clear();
}


// ------------------------------------------
void CRenderManager::renderShadowCasters(CHandle h_light, int room) {
	CTraceScoped scope("Shadow Casters");
	PROFILE_FUNCTION("SHADOW CASTERS OBJ");

	if (!in_order_shadows) {
		// sort the keys based on....
		for (int idx = 0; idx < ROOMS_SIZE; idx++) {
			std::sort(all_shadow_keys[idx].begin(), all_shadow_keys[idx].end(), &sortByMesh);
		}
		in_order_shadows = true;
		++ntimes_sorted;
	}

		
	TShadowKey* it = &all_shadow_keys[room][0];
	TShadowKey* end_it = &all_shadow_keys[room][all_shadow_keys[room].size()-1];


	static TShadowKey null_key;
	memset(&null_key, 0x00, sizeof(TShadowKey));
	const TShadowKey* prev_it = &null_key;

	// Check if we have culling information from the camera source
	CEntity* e_camera = h_light;
	TCompRoom* cam_room = e_camera->get<TCompRoom>();

	auto i = find(cam_room->all_rooms.begin(), cam_room->all_rooms.end(), room);

	if (i == cam_room->all_rooms.end()) {
		return;
	}

	TCompCulling::TCullingBits* culling_bits = nullptr;
	TCompCulling* culling = nullptr;

	if (e_camera)
		culling = e_camera->get<TCompCulling>();
	if (culling)
		culling_bits = &culling->bits;

	// To get the index of each aabb
	auto hm_aabbs = getHandleManager<TCompAbsAABB>();
	const TCompAbsAABB* base_aabbs = hm_aabbs->getFirstObject();
	int nkeys_rendered = 0;

	bool valid = false;
	while (true) {
		PROFILE_FUNCTION("SHADOW CASTERS OBJ: while");
		
		valid = true;

		const TCompTransform* c_tmx = it->transform;

		//assert(c_tmx);
		if (c_tmx) {
			PROFILE_FUNCTION("SHADOW CASTERS OBJ: render and activate");
				
			if (culling_bits) {
				TCompAbsAABB* aabb = it->aabb;
				if (aabb) {
					PROFILE_FUNCTION("SHADOW CASTERS OBJ: culling");
					intptr_t idx = aabb - base_aabbs;
					if (!culling_bits->test(idx)) {
						valid = false;
					}
					nkeys_rendered++;
				}
			}
			if (valid) {
				activateWorldMatrix(c_tmx->asMatrix());
				// If the shadows_keys were sorted by mesh
				// I could skip the activation and just call it
				// when the mesh changed, and only call the render
				if (it->mesh != prev_it->mesh) {
					it->mesh->activate();
				}
				else {
					int i = 0;
				}
				it->mesh->render();
				prev_it = it;
			}
		}
		else {
			//Puede no tener transform, ignoralo y ya esta
			//fatal("render__manager: tranfrom from shadowcaster null");
		}


		if (it != end_it) {
			++it;
		}
		else {
			renderedCulling.push_back(nkeys_rendered);
			return;
		}
	}

	
}

// ------------------------------------------

//render shadowcaster with skinning tech
void CRenderManager::renderShadowCastersSkin(CHandle h_light, int room) {
	PROFILE_FUNCTION("SHADOW CASTER SKIN");

	CEntity* e_camera = h_light;

	if (!in_order_shadows_skin) {
		// sort the keys based on....
		for (int idx = 0; idx < ROOMS_SIZE; idx++) {
			std::sort(all_shadow_skinning_keys[idx].begin(), all_shadow_skinning_keys[idx].end(), &sortByMesh);
		}
		in_order_shadows_skin = true;
		++ntimes_sorted;
	}


	TShadowKey* it = &all_shadow_skinning_keys[room][0];
	TShadowKey* end_it = &all_shadow_skinning_keys[room][all_shadow_skinning_keys[room].size()-1];

	static TShadowKey null_key;
	memset(&null_key, 0x00, sizeof(TShadowKey));
	const TShadowKey* prev_it = &null_key;

	// Check if we have culling information from the camera source

	TCompRoom* cam_room = e_camera->get<TCompRoom>();

	auto i = find(cam_room->all_rooms.begin(), cam_room->all_rooms.end(), room);

	if (i == cam_room->all_rooms.end()) {
		return;
	}


	TCompCulling::TCullingBits* culling_bits = nullptr;
	TCompCulling* culling = nullptr;

	if (e_camera)
		culling = e_camera->get<TCompCulling>();
	if (culling)
		culling_bits = &culling->bits;

	auto hm_aabbs = getHandleManager<TCompAbsAABB>();
	const TCompAbsAABB* base_aabbs = hm_aabbs->getFirstObject();


	bool valid = true;
	while (true) {
		valid = true;
			const TCompTransform* c_tmx = it->transform;

			if (c_tmx) {


				if (culling_bits) {
					TCompAbsAABB* aabb = it->aabb;
					if (aabb) {
						PROFILE_FUNCTION("SHADOW CASTERS OBJ: culling");
						intptr_t idx = aabb - base_aabbs;
						if (!culling_bits->test(idx)) {
							valid = false;
						}
					}
				}

				

				const CEntity* e = it->owner.getOwner();
				if (!e) {
					valid = false;
				}
				const TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
				if(!comp_skel) {
					valid = false;
				}

				if (valid) {
					activateWorldMatrix(c_tmx->asMatrix());
					comp_skel->uploadBonesToCteShader();

					// If the shadows_keys were sorted by mesh
					// I could skip the activation and just call it
					// when the mesh changed, and only call the render
					if (it->mesh != prev_it->mesh) {
						it->mesh->activate();
					}
					it->mesh->render();
					prev_it = it;
				}
			}
			else {
				//fatal("render__manager: tranfrom from shadowcaster skinning null");
			}

			if (it != end_it) {
				++it;
			}
			else {
				return;
			}
	}
}