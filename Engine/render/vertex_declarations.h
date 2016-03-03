#ifndef INC_RENDER_VERTEX_DECLARATIONS_H_
#define INC_RENDER_VERTEX_DECLARATIONS_H_

#include "render.h"

struct CVertexDeclaration {
  const D3D11_INPUT_ELEMENT_DESC* elems;
  uint32_t                        nelems;
  const char*                     name;
  uint32_t                        id;
  uint32_t                        bytes_per_vertex;
  CVertexDeclaration(const D3D11_INPUT_ELEMENT_DESC* new_elems
    , uint32_t new_nelems
    , const char* new_name
    , uint32_t new_id
    , uint32_t new_bytes_per_vertex
    );
};

class CVertexDeclarationMgr {
public:
  std::vector< CVertexDeclaration* > all_decls;
  const CVertexDeclaration* getByName(const std::string& name);
  const CVertexDeclaration* getById( uint32_t id );
};

extern CVertexDeclarationMgr vdecl_manager;

#endif

