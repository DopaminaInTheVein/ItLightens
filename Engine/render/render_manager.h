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
  };

  static bool sortByTechMatMesh(const TKey& k1, const TKey& k2);
  
  bool in_order;
  std::vector< TKey > all_keys;

public:
  
  void registerToRender(const CStaticMesh* mesh, CHandle handle);
  void unregisterFromRender(CHandle handle);
  void renderAll();

};

extern CRenderManager RenderManager;

#endif

