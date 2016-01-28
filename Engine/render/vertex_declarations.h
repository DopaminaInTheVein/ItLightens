#ifndef INC_RENDER_VERTEX_DECLARATIONS_H_
#define INC_RENDER_VERTEX_DECLARATIONS_H_

#include "render.h"

struct CVertexDeclaration {
  const D3D11_INPUT_ELEMENT_DESC* elems;
  uint32_t                        nelems;
  const char*                     name;
  CVertexDeclaration(const D3D11_INPUT_ELEMENT_DESC* new_elems
    , uint32_t new_nelems
    , const char* new_name
    ) 
  : elems( new_elems )
  , nelems( new_nelems )
  , name( new_name )
  { }
};

extern CVertexDeclaration vdecl_positions;
extern CVertexDeclaration vdecl_positions_color;
extern CVertexDeclaration vdecl_positions_uv;

#endif

