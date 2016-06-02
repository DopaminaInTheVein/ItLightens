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
#include "render/draw_utils.h"

CRenderManager RenderManager;

#include "render/shader_cte.h"

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
    if (tech1->getPriority() == tech2->getPriority())
      return tech1->getName() < tech2->getName();
    return (tech1->getPriority() < tech2->getPriority());
  }
  return k1.material < k2.material;
}

void CRenderManager::registerToRender(const CStaticMesh* mesh, CHandle owner) {
  CEntity* e = owner.getOwner();
  assert(e);
  CHandle h_transform = e->get<TCompTransform>();
  CHandle h_aabb = e->get<TCompAbsAABB>();

  for (auto s : mesh->slots) {
    TKey k;
    k.material = s.material;
    k.mesh = s.mesh;
    k.owner = owner;
    k.transform = h_transform;
    k.submesh_idx = s.submesh_idx;
    k.aabb = h_aabb;
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
      all_shadow_keys.push_back(k);
    }
  }

  in_order = false;
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

void CRenderManager::renderAll(CHandle h_camera, CRenderTechnique::eCategory category) {
  char cat_name[32];
  if (category == CRenderTechnique::DBG_OBJS)
    strcpy(cat_name, "DEBUG_OBJS");
  else if (category == CRenderTechnique::SOLID_OBJS)
    strcpy(cat_name, "SOLID_OBJS");
  else if (category == CRenderTechnique::TRANSPARENT_OBJS)
    strcpy(cat_name, "TRANSPARENT_OBJS");
  else if (category == CRenderTechnique::UI_OBJS)
    strcpy(cat_name, "UI_OBJS");
  PROFILE_FUNCTION(cat_name);
  CTraceScoped scope(cat_name);

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
  if( e_camera )
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
  auto d1 = std::distance( all_keys.begin(), r.second);
  const TKey* it = &all_keys[0] + d0;
  const TKey* end_it = &all_keys[0] + d1;
  
  static TKey null_key;
  memset(&null_key, 0x00, sizeof(TKey));
  const TKey* prev_it = &null_key;

  bool curr_tech_used_bones = false;
  int  nkeys_rendered = 0;

  // Pasearse por todas las keys
  while (it != end_it) {

    // Do the culling
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

    if (it->material != prev_it->material) {
      if (!prev_it->material || it->material->tech != prev_it->material->tech) {
        it->material->tech->activate();
        curr_tech_used_bones = it->material->tech->usesBones();
      }
      it->material->activateTextures();
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

    if (curr_tech_used_bones) {
      const CEntity* e = it->owner.getOwner();
      assert(e);
      const TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
      assert(comp_skel);
      comp_skel->uploadBonesToCteShader();
    }

    it->mesh->renderGroup(it->submesh_idx);    // it->mesh->renderSubMesh( it->submesh );
    prev_it = it;
    ++it;
    ++nkeys_rendered;
  }

  CMaterial::deactivateTextures();

  renderedCulling.push_back(nkeys_rendered);
}

void CRenderManager::renderUICulling() {
	for (int i = 0; i < renderedCulling.size(); i++) {
		ImGui::Text("%d/%ld keys of call %d", renderedCulling[i], all_keys.size(), i);
	}

	renderedCulling.clear();
}

// ------------------------------------------
void CRenderManager::renderShadowCasters() {
  auto it = all_shadow_keys.begin();
  while (it != all_shadow_keys.end()) {

    const TCompTransform* c_tmx = it->transform;
    //TODO: Review Pedro!
	//assert(c_tmx);
	if (c_tmx) {
		activateWorldMatrix(c_tmx->asMatrix());

		// If the shadows_keys were sorted by mesh
		// I could skip the activation and just call it
		// when the mesh changed, and only call the render
		it->mesh->activateAndRender();
	}else{
		fatal("render__manager: tranfrom from shadowcaster null");

	}
	++it;
  }
}
