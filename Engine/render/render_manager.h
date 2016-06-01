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
    int                     submesh_idx;
    //const CRenderTechnique* tech;
    CHandle                 owner;
    CHandle                 transform;
    CHandle                 aabb;
  };
private:

  struct TShadowKey {
    const CMesh*            mesh;
    CHandle                 owner;
    CHandle                 transform;
  };
  static bool sortByTechMatMesh(const TKey& k1, const TKey& k2);

  bool in_order;
  std::vector< TKey > all_keys;
  std::vector< TShadowKey > all_shadow_keys;
  std::vector<int> renderedCulling;

  int  ntimes_sorted = 0;
public:

  void registerToRender(const CStaticMesh* mesh, CHandle handle);
  void unregisterFromRender(CHandle handle);
  void renderUICulling();
  void renderAll(CHandle h_camera, CRenderTechnique::eCategory category);
  void renderShadowCasters();
};

extern CRenderManager RenderManager;

#endif
