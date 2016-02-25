#include "mcv_platform.h"
#include "render_manager.h"
#include "static_mesh.h"
#include "material.h"
#include "technique.h"
#include "mesh.h"
#include "components/comp_transform.h"
#include "components/entity.h"

CRenderManager RenderManager;

#include "render/shader_cte.h"
#include "contants/ctes_object.h"
extern CShaderCte< TCteObject > shader_ctes_object;

bool CRenderManager::sortByTechMatMesh(
	const TKey &k1
	, const TKey &k2) {
	if (k1.material->tech != k2.material->tech) {
		if (k1.material->tech->getPriority() == k2.material->tech->getPriority())
			return k1.material->tech->getName() < k2.material->tech->getName();
		return (k1.material->tech->getPriority() < k2.material->tech->getPriority());
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

void CRenderManager::renderAll() {
	if (!in_order) {
		// sort the keys based on....
		std::sort(all_keys.begin(), all_keys.end(), &sortByTechMatMesh);
		in_order = true;
	}

	if (all_keys.empty())
		return;

	//
	auto it = all_keys.begin();
	auto prev_it = it;

	// Activate the first key to avoid checking for validations
	// on each iteration
	{
		it->material->tech->activate();
		it->material->activate();
		it->mesh->activate();
		TCompTransform* c_tmx = it->transform;
		assert(c_tmx);
		shader_ctes_object.World = c_tmx->asMatrix();
		shader_ctes_object.uploadToGPU();
		shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
		it->mesh->render();    // it->mesh->renderSubMesh( it->submesh );
		++it;
	}

	// Pasearse por todas las keys
	while (it != all_keys.end()) {
		if (it->material != prev_it->material) {
			if (it->material->tech != prev_it->material->tech) {
				it->material->tech->activate();
			}
			it->material->activate();
		}
		if (it->mesh != prev_it->mesh) {
			it->mesh->activate();
		}

		if (it->owner != prev_it->owner) {
			// subir la world de it
			TCompTransform* c_tmx = it->transform;
			assert(c_tmx);
			shader_ctes_object.World = c_tmx->asMatrix();
			shader_ctes_object.uploadToGPU();
			shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
		}

		it->mesh->render();    // it->mesh->renderSubMesh( it->submesh );
		prev_it = it;
		++it;
	}
}