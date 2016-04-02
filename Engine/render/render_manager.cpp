#include "mcv_platform.h"
#include "render_manager.h"
#include "static_mesh.h"
#include "material.h"
#include "technique.h"
#include "mesh.h"
#include "components/comp_transform.h"
#include "skeleton/comp_skeleton.h"
#include "components/entity.h"

CRenderManager RenderManager;

#include "render/shader_cte.h"
#include "contants/ctes_object.h"
extern CShaderCte< TCteObject > shader_ctes_object;

bool CRenderManager::sortByTechMatMesh(
	const TKey &k1
	, const TKey &k2) {
	PROFILE_FUNCTION("RM: sortByTechMatMesh");
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
	PROFILE_FUNCTION("RenderManager: renderALL");
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
		it->material->activateTextures();
		it->mesh->activate();
		TCompTransform* c_tmx = it->transform;
		assert(c_tmx);
		shader_ctes_object.World = c_tmx->asMatrix();
		shader_ctes_object.Rotation = MAT44::CreateFromQuaternion(c_tmx->getRotation());
		shader_ctes_object.uploadToGPU();
		shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);

		//if (it->material->tech->usesBones()) {
		//	CEntity* e = it->owner.getOwner();
		//	assert(e);
		//	TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
		//	assert(comp_skel);
		//	comp_skel->uploadBonesToCteShader();
		//}

		it->mesh->render();    // it->mesh->renderSubMesh( it->submesh );
		++it;
	}

	// Pasearse por todas las keys
	while (it != all_keys.end()) {
		if (it->material != prev_it->material) {
			if (it->material->tech != prev_it->material->tech)
				it->material->tech->activate();
			it->material->activateTextures();
		}
		if (it->mesh != prev_it->mesh)
			it->mesh->activate();

		if (it->owner != prev_it->owner) {
			// subir la world de it
			TCompTransform* c_tmx = it->transform;
			assert(c_tmx);

			// For static objects, we could skip this step
			// if each static object had it's own shader_ctes_object
			shader_ctes_object.Rotation = MAT44::CreateFromQuaternion(c_tmx->getRotation());
			shader_ctes_object.World = c_tmx->asMatrix();
			shader_ctes_object.uploadToGPU();

			// We could skip this step as currently, we only
			// have one shader_ctes_object
			shader_ctes_object.activate(CTE_SHADER_OBJECT_SLOT);
		}

		it->mesh->render();    // it->mesh->renderSubMesh( it->submesh );
		if (it->material->tech->usesBones()) {
			CEntity* e = it->owner.getOwner();
			assert(e);
			TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
			assert(comp_skel);
			comp_skel->uploadBonesToCteShader();
		}

		prev_it = it;
		++it;
	}

	CMaterial::deactivateTextures();
}