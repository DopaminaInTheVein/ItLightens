#include "mcv_platform.h"
#include "vertex_declarations.h"

#define DECL_VERTEX_DECLARATION( name ) CVertexDeclaration vdecl_##name(layout_##name, ARRAYSIZE(layout_##name), #name)


// ------------------------------------
// Define the input layout
D3D11_INPUT_ELEMENT_DESC layout_positions[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
DECL_VERTEX_DECLARATION(positions);

// ------------------------------------
D3D11_INPUT_ELEMENT_DESC layout_positions_color[] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  // 12 = 3*sizeof(float) = x,y,z (float)
  { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
DECL_VERTEX_DECLARATION(positions_color);



