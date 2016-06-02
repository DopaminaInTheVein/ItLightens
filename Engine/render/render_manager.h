#ifndef INC_RENDER_MANAGER_H_
#define INC_RENDER_MANAGER_H_

#include "handle/handle.h"

class CMesh;
class CRenderTechnique;
class CMaterial;
class CStaticMesh;

class CRenderManager {
<<<<<<< HEAD
=======
public:
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
  struct TKey {
    const CMesh*            mesh;
    const CMaterial*        material;
    int                     submesh_idx;
    //const CRenderTechnique* tech;
    CHandle                 owner;
    CHandle                 transform;
    CHandle                 aabb;
<<<<<<< HEAD
    //int                     aabb_idx;
=======
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
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

  int  ntimes_sorted = 0;
public:

  void registerToRender(const CStaticMesh* mesh, CHandle handle);
  void unregisterFromRender(CHandle handle);

<<<<<<< HEAD
  enum eRenderType {
    SOLID_OBJS
    , TRANSPARENT_OBJS
  };

  void renderAll(CHandle h_camera, eRenderType render_type);
=======
  void renderAll(CHandle h_camera, CRenderTechnique::eCategory category);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427
  void renderShadowCasters();
};

extern CRenderManager RenderManager;

#endif
