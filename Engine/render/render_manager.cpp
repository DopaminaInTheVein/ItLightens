#include "mcv_platform.h"
#include "render_manager.h"
#include "static_mesh.h"
#include "material.h"
#include "technique.h"
#include "mesh.h"
#include "components/comp_transform.h"
#include "skeleton/comp_skeleton.h"
#include "components/entity.h"
#include "render/draw_utils.h"

CRenderManager RenderManager;

#include "render/shader_cte.h"

bool CRenderManager::sortByTechMatMesh(
  const TKey &k1
  , const TKey &k2) {
  auto* tech1 = k1.material->tech;
  auto* tech2 = k2.material->tech;
  if (tech1 != tech2) {
    if (tech1->isTransparent() != tech2->isTransparent())
      return tech1->isTransparent();
    if (tech1->getPriority() == tech2->getPriority())
      return tech1->getName() < tech2->getName();
    return (tech1->getPriority() < tech2->getPriority());
  }
  // TODO: hacer esto bien...
  return k1.material < k2.material;
}

void CRenderManager::registerToRender(const CStaticMesh* mesh, CHandle owner) {
  CEntity* e = owner.getOwner();
  assert(e);
  CHandle h_transform = e->get<TCompTransform>();

  for (auto s : mesh->slots) {
    TKey k;
    k.material = s.material;
    k.mesh = s.mesh;
    k.owner = owner;
    k.transform = h_transform;
    k.submesh_idx = s.submesh_idx;
    all_keys.push_back(k);
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

void CRenderManager::renderAll(eRenderType render_type) {
  PROFILE_FUNCTION("RenderManager");
  CTraceScoped scope("RenderManager");

  if (!in_order) {
    // sort the keys based on....
    std::sort(all_keys.begin(), all_keys.end(), &sortByTechMatMesh);
    in_order = true;
  }

  if (all_keys.empty())
    return;

  //
  const TKey* it = nullptr;
  const TKey* end_it = nullptr;

  auto it_first_solid = std::lower_bound(
    all_keys.begin()
    , all_keys.end()
    , true
    , [](const TKey &k1, bool is_transparent)->bool {
    return k1.material->tech->isTransparent();
  }
  );

  if (render_type == eRenderType::TRANSPARENT_OBJS) {
    it = &all_keys[0];
    end_it = &(*it_first_solid);
  }
  else {
    it = &(*it_first_solid);
    int idx_of_first_solid = std::distance(all_keys.begin(), it_first_solid);
    int num_solid_keys = all_keys.size() - idx_of_first_solid;
    end_it = it + num_solid_keys;
  }

  static TKey null_key;
  memset(&null_key, 0x00, sizeof(TKey));
  const TKey* prev_it = &null_key;

  bool curr_tech_used_bones = false;

  // Pasearse por todas las keys
  while (it != end_it) {
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
  }

  CMaterial::deactivateTextures();
}