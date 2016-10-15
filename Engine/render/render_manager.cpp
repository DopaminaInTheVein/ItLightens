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
//sorters

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

#include "components\comp_camera_main.h"
struct CRenderManager::sortByTechDistance
{
	CHandle camera;
	sortByTechDistance(CHandle cam) { camera = cam; }
	bool operator()(const TKey & k1, const TKey & k2) {
		auto* tech1 = k1.material->tech;
		auto* tech2 = k2.material->tech;

		//sort by technique
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

		//sort by distance
		CHandle h_k1 = k1.owner.getOwner();
		CHandle h_k2 = k2.owner.getOwner();
		//CEntity * e_cam = camera;
		GET_COMP(cam, camera, TCompCameraMain);
		if (!cam || !h_k1.isValid() || !h_k2.isValid())
			return 0;

		VEC3 cam_pos = cam->getPosition();

		GET_COMP(t_k1, h_k1, TCompTransform);
		GET_COMP(t_k2, h_k1, TCompTransform);
		if (!t_k1 || !t_k2)
			return 0;

		float dist_to_k1 = simpleDist(t_k1->getPosition(), cam_pos);
		float dist_to_k2 = simpleDist(t_k2->getPosition(), cam_pos);

		return dist_to_k1 > dist_to_k2;
	}
};

// -------------------------------------------------
bool CRenderManager::sortByMesh(
	const TShadowKey &k1
	, const TShadowKey &k2) {
	PROFILE_FUNCTION("Render manager: sort");

	//shadow caster are sorted by mesh
	return k1.mesh < k2.mesh;
}

#include "components\comp_render_static_mesh.h"
void CRenderManager::registerToRender(const CStaticMesh* mesh, CHandle owner) {
	bool isDynamic = true;
	if (owner.isValid()) {
		TCompRenderStaticMesh* rsm = owner;
		isDynamic = rsm->isDynamic();
	}

	CHandle ow = owner.getOwner();
	//assert(ow.isValid());
	if (!ow.isValid()) return;

	CHandle h_transform = GETH_COMP(ow, TCompTransform);//e->get<TCompTransform>();
	CHandle h_aabb = GETH_COMP(ow, TCompAbsAABB);//e->get<TCompAbsAABB>();
	TRoom oroom;
	GET_COMP(comproom, ow, TCompRoom);
	if (comproom) oroom = comproom->getRoom();
	std::vector<TKey> * render_list[ROOMS_SIZE];

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

		if (k.material->tech->getCategory() != CRenderTechnique::DETAIL_OBJS && k.material->tech->getCategory() != CRenderTechnique::TRANSPARENT_OBJS) {
			for (int idx = 0; idx < ROOMS_SIZE; idx++) {
				render_list[idx] = &all_keys[idx];
			}
		}
		else {
			for (int idx = 0; idx < ROOMS_SIZE; idx++) {
				render_list[idx] = &dynamic_keys[idx];
			}
		}
		if (oroom.empty() || playercandidate) {
			for (int idx = 0; idx < ROOMS_SIZE; idx++) {
				render_list[idx]->push_back(k);
			}
		}
		else {
			for (auto room : oroom.getList()) {
				if (room >= 0 && room < ROOMS_SIZE)
					render_list[room]->push_back(k);
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
			k.isDynamic = isDynamic;

			if (!s.material->tech->usesBones()) {
				for (auto room : oroom.getList()) {
					if (room >= ROOMS_SIZE) continue;
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
				for (auto room : oroom.getList()) {
					if (room >= ROOMS_SIZE) continue;
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

	for (int idx = 0; idx < ROOMS_SIZE; idx++) {
		auto it = dynamic_keys[idx].begin();
		while (it != dynamic_keys[idx].end()) {
			if (it->owner == owner) {
				num_renders--;
				it = dynamic_keys[idx].erase(it);
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

void CRenderManager::renderDynamic(CHandle h_camera, CRenderTechnique::eCategory category, int room) {
	//sort objects by the actual distance
	std::sort(dynamic_keys[room].begin(), dynamic_keys[room].end(), sortByTechDistance(h_camera));

	//start render objects from list
	renderList(h_camera, category, room, dynamic_keys[room]);
}

void CRenderManager::renderList(CHandle h_camera, CRenderTechnique::eCategory category, int room, std::vector<TKey> & render_list)
{
	if (render_list.empty())
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
		render_list.begin()
		, render_list.end()
		, category
	);
	auto d0 = std::distance(render_list.begin(), r.first);
	auto d1 = std::distance(render_list.begin(), r.second);
	TKey* it = &render_list[0] + d0;
	TKey* end_it = &render_list[0] + d1;

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
		//GET_COMP(tentroom, it->owner.getOwner(), TCompRoom);
		//if (tentroom) it->room = tentroom->getRoom();
		//if (it->owner.getOwner() == CPlayerBase::handle_player || it->room.sameRoom(pj_room)) {
			if (culling_bits) {
				PROFILE_FUNCTION("Culling");
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
				PROFILE_FUNCTION("Polarity player");
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
				PROFILE_FUNCTION("Polarity other");
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
				PROFILE_FUNCTION("Change material");
				if (!prev_it->material || it->material->tech != prev_it->material->tech) {
					it->material->tech->activate();
					curr_tech_used_bones = it->material->tech->usesBones();
				}
				it->material->activateTextures(it->polarity);
			}
			else {
				PROFILE_FUNCTION("Same material");
				if (it->polarity != prev_it->polarity) {
					it->material->activateTextures(it->polarity);
				}
			}
			if (it->mesh != prev_it->mesh) {
				PROFILE_FUNCTION("Mesh activate");
				it->mesh->activate();
			}
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
				PROFILE_FUNCTION("Bones");
				if (renderSkeleton(it)) {
					//valid skeleton
					it->mesh->renderGroup(it->submesh_idx);
					prev_it = it;
					++nkeys_rendered;
				}
			}

			//render UI object
			else if (it->material->tech->getCategory() == CRenderTechnique::UI_OBJS) {
				PROFILE_FUNCTION("Ui object");
				if (renderUI(it)) {
					//Valid UI
					it->mesh->renderGroup(it->submesh_idx);
					prev_it = it;
					++nkeys_rendered;

					//CEntity* e_owner = it->owner.getOwner();
					//TCompName* e_name = e_owner->get<TCompName>();
					//{
					//	PROFILE_FUNCTION("Test_z push_back");
					//	test_z_render.push_back(e_name->name);
					//	dbg("test z_render size: %d\n", test_z_render.size());
					//}
				}
			}
			else {
				PROFILE_FUNCTION("Desfault seleton");
				//default skeleton
				it->mesh->renderGroup(it->submesh_idx);
				prev_it = it;
				++nkeys_rendered;
			}
		//}

		++it;
	}

#ifndef NDEBUG
	//if (ui_render)checkTestZ(last_test_z_render, test_z_render);
#endif
	{
		PROFILE_FUNCTION("Deactivate textures");
		CMaterial::deactivateTextures();
	}
	renderedCulling.push_back(nkeys_rendered);
}

void CRenderManager::renderAll(CHandle h_camera, CRenderTechnique::eCategory category, int room) {
	std::string name;
	ui_render = false;
	dynamic_render = false;
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
		dynamic_render = true;
		name = "TRANSPARENT_OBJS";
		PROFILE_FUNCTION("TRANSPARENT_OBJS");
	}
	else if (category == CRenderTechnique::UI_OBJS) {
		name = "UI_OBJS";
		PROFILE_FUNCTION("UI_OBJS");
		ui_render = true;
	}
	else if (category == CRenderTechnique::DETAIL_OBJS) {
		dynamic_render = true;
		name = "DETAIL_OBJS";
		PROFILE_FUNCTION("DETAIL_OBJS");
	}
	else {
		name = "UNKNOWN_OBJS";
		PROFILE_FUNCTION("UNKNOWN_OBJS");
	}
	CTraceScoped scope(name.c_str());

	if (dynamic_render) {
		renderDynamic(h_camera, category, room);
		return;
	}

	if (!in_order) {
		// sort the keys based on....

		for (int idx = 0; idx < ROOMS_SIZE; idx++) {
			std::sort(all_keys[idx].begin(), all_keys[idx].end(), &sortByTechMatMesh);
		}
		in_order = true;
		++ntimes_sorted;
	}

	//render objects
	renderList(h_camera, category, room, all_keys[room]);
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
	PROFILE_FUNCTION("Render UI");
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

void CRenderManager::renderStaticShadowCasters(CHandle h_light, int room) {
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
	TShadowKey* end_it = &all_shadow_keys[room][all_shadow_keys[room].size() - 1];

	static TShadowKey null_key;
	memset(&null_key, 0x00, sizeof(TShadowKey));
	const TShadowKey* prev_it = &null_key;

	// Check if we have culling information from the camera source
	CEntity* e_camera = h_light;
	TCompRoom* cam_room = e_camera->get<TCompRoom>();

	if (ROOM_IS_IN(cam_room, room)) return;
	//auto i = find(cam_room->name.begin(), cam_room->name.end(), room);

	//if (i == cam_room->name.end()) {
	//	return;
	//}

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

		if (it->isDynamic) {
			if (it != end_it) {
				++it;
				continue;
			}
			else {
				return;
			}
		}

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
void CRenderManager::renderShadowCasters(CHandle h_light, int room) {
	CTraceScoped scope("Shadow Casters");
	PROFILE_FUNCTION("SHADOW CASTERS OBJ");

	if (all_shadow_keys[room].empty()) {
		return;
	}

	if (!in_order_shadows) {
		// sort the keys based on....
		for (int idx = 0; idx < ROOMS_SIZE; idx++) {
			std::sort(all_shadow_keys[idx].begin(), all_shadow_keys[idx].end(), &sortByMesh);
		}
		in_order_shadows = true;
		++ntimes_sorted;
	}

	TShadowKey* it = &all_shadow_keys[room][0];
	TShadowKey* end_it = &all_shadow_keys[room][all_shadow_keys[room].size() - 1];

	static TShadowKey null_key;
	memset(&null_key, 0x00, sizeof(TShadowKey));
	const TShadowKey* prev_it = &null_key;

	// Check if we have culling information from the camera source
	CEntity* e_camera = h_light;
	TCompRoom* cam_room = e_camera->get<TCompRoom>();

	if (ROOM_IS_IN(cam_room, room)) return;
	//auto i = find(cam_room->name.begin(), cam_room->name.end(), room);

	//if (i == cam_room->name.end()) {
	//	return;
	//}

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

		if (!it->isDynamic) {
			if (it != end_it) {
				++it;
				continue;
			}
			else {
				return;
			}
		}

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
	TShadowKey* end_it = &all_shadow_skinning_keys[room][all_shadow_skinning_keys[room].size() - 1];

	static TShadowKey null_key;
	memset(&null_key, 0x00, sizeof(TShadowKey));
	const TShadowKey* prev_it = &null_key;

	// Check if we have culling information from the camera source

	TCompRoom* cam_room = e_camera->get<TCompRoom>();
	if (ROOM_IS_IN(cam_room, room)) return;
	//auto i = find(cam_room->name.begin(), cam_room->name.end(), room);

	//if (i == cam_room->name.end()) {
	//	return;
	//}

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
			if (!comp_skel) {
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

std::string CRenderManager::TKey::print() {
#ifndef FINAL_BUILD
	char text[1024];
	sprintf(text, "Mesh:%s\nMaterial:%s\nPolarity:%d, HOwner:%s, Room:%s, isPlayer:%d"
		, mesh ? mesh->getName() : "unknown"
		, material ? material->getName() : "unknown"
		, polarity
		, GET_NAME(owner.getOwner())
		, room.print()
		, isPlayer
	);
	return text;
#endif
}

std::string CRenderManager::TShadowKey::print() {
#ifndef FINAL_BUILD
	char text[1024];
	sprintf(text, "Mesh:%s\n, HOwner:%s\n, Room:%d\n"
		, mesh ? mesh->getName() : "unknown"
		, GET_NAME(owner.getOwner())
		, room.print()
	);
	return text;
#endif
}