#ifndef INC_RENDER_MANAGER_H_
#define INC_RENDER_MANAGER_H_

#include "handle/handle.h"

#define ROOMS_SIZE 3

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

	struct TShadowKey {
		const CMesh*            mesh;
		CHandle                 owner;
		CHandle                 transform;
		CHandle                 aabb;
		std::vector<int>		room;
		bool                    isPlayer;
	};

private:

	int num_renders = 0;

	static bool sortByTechMatMesh(const TKey & k1, const TKey & k2);
	static bool sortByMesh(const TShadowKey & k1, const TShadowKey & k2);

	bool in_order;
	bool in_order_shadows;
	bool in_order_shadows_skin;
	std::vector< TKey > all_keys[ROOMS_SIZE];
	std::vector< TShadowKey > all_shadow_keys[ROOMS_SIZE];

	//will use another tech for skinning from defaul shadowcasters
	std::vector< TShadowKey > all_shadow_skinning_keys[ROOMS_SIZE];

	std::vector<int> renderedCulling;

	//std::vector<std::string> test_z_render;
	//std::vector<std::string> last_test_z_render;

	int  ntimes_sorted = 0;
public:

	void registerToRender(const CStaticMesh* mesh, CHandle handle);
	void unregisterFromRender(CHandle handle);
	void renderUICulling(int room);
	void renderAll(CHandle h_camera, CRenderTechnique::eCategory category, int room = 0);
	bool renderSkeleton(TKey * it);
	bool renderUI(TKey * it);
	void renderShadowCasters(CHandle h_light, int room);
	void renderShadowCastersSkin(CHandle h_light, int room);

	void ModifyUI() {
		in_order = false;
	}
};

extern CRenderManager RenderManager;

#endif
