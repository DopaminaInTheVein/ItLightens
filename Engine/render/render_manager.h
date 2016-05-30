#ifndef INC_RENDER_MANAGER_H_
#define INC_RENDER_MANAGER_H_

#include "handle/handle.h"

class CMesh;
class CRenderTechnique;
class CMaterial;
class CStaticMesh;

class CRenderManager {

  struct TKey {
    const CMesh*            mesh;
    const CMaterial*        material;
    int                     submesh_idx;
    //const CRenderTechnique* tech;
    CHandle                 owner;
    CHandle                 transform;
    CHandle                 aabb;
    //int                     aabb_idx;
  };

  struct TShadowKey {
    const CMesh*            mesh;
    CHandle                 owner;
    CHandle                 transform;
  };
  static bool sortByTechMatMesh(const TKey& k1, const TKey& k2);
  static bool sortByTransparency(const TKey &k1, bool is_transparent);

  bool in_order;
  std::vector< TKey > all_keys;
  std::vector< TShadowKey > all_shadow_keys;

  int  ntimes_sorted = 0;
  std::vector<int> renderedCulling;
public:
  
  void registerToRender(const CStaticMesh* mesh, CHandle handle);
  void unregisterFromRender(CHandle handle);

  enum eRenderType {
    SOLID_OBJS
    , TRANSPARENT_OBJS
  };

  void renderAll(CHandle h_camera, eRenderType render_type);
  void renderUICulling();
  void renderShadowCasters();
  void clear() {
	  all_keys.clear();
	  all_shadow_keys.clear();
	  renderedCulling.clear();

  }
};

extern CRenderManager RenderManager;

#endif

