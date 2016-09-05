#ifndef INC_RENDER_MANAGER_H_
#define INC_RENDER_MANAGER_H_

#include "handle/handle.h"

class CMesh;
class CRenderTechnique;
class CMaterial;
class CStaticMesh;

class CRenderManager {
public:
	struct TKey {
		const CMesh*            mesh;
		const CMaterial*        material;
		int						polarity;
		int                     submesh_idx;
		//const CRenderTechnique* tech;
		CHandle                 owner;
		CHandle                 transform;
		CHandle                 aabb;
		std::vector<int>        room;
		bool                    isPlayer;
	};
private:

	struct TShadowKey {
		const CMesh*            mesh;
		CHandle                 owner;
		CHandle                 transform;
		CHandle                 aabb;
		std::vector<int>		room;
		bool                    isPlayer;
	};

	static bool sortByTechMatMesh(const TKey & k1, const TKey & k2);

	bool in_order;
	std::vector< TKey > all_keys;
	std::vector< TShadowKey > all_shadow_keys;

	//will use another tech for skinning from defaul shadowcasters
	std::vector< TShadowKey > all_shadow_skinning_keys;

	std::vector<int> renderedCulling;

	int  ntimes_sorted = 0;
public:

	void registerToRender(const CStaticMesh* mesh, CHandle handle);
	void unregisterFromRender(CHandle handle);
	void renderUICulling();
	void renderAll(CHandle h_camera, CRenderTechnique::eCategory category);
	void renderSkeleton(TKey * it);
	void renderUI(TKey * it);
	void renderShadowCasters(CHandle h_light);
	void renderShadowCastersSkin(CHandle h_light);
};

extern CRenderManager RenderManager;

#endif
