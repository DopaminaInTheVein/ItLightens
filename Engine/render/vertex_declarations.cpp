#include "mcv_platform.h"
#include "vertex_declarations.h"
#include "mesh.h"

#define DECL_VERTEX_DECLARATION( name, id, bytes ) CVertexDeclaration vdecl_##name(layout_##name, ARRAYSIZE(layout_##name), #name, id, bytes)

CVertexDeclarationMgr vdecl_manager;

CVertexDeclaration::CVertexDeclaration(const D3D11_INPUT_ELEMENT_DESC* new_elems
  , uint32_t new_nelems
  , const char* new_name
  , uint32_t new_id
  , uint32_t new_bytes_per_vertex
  )
  : elems(new_elems)
  , nelems(new_nelems)
  , name(new_name)
  , id(new_id)
  , bytes_per_vertex(new_bytes_per_vertex)
{ 
  vdecl_manager.all_decls.push_back(this);
}

// ------------------------------------
// Define the input layout
D3D11_INPUT_ELEMENT_DESC layout_positions[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
DECL_VERTEX_DECLARATION(positions, CMesh::VTX_DECL_POSITION, 3*4 );

// ------------------------------------
D3D11_INPUT_ELEMENT_DESC layout_positions_color[] =
{
  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  // 12 = 3*sizeof(float) = x,y,z (float)
  { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
DECL_VERTEX_DECLARATION(positions_color, CMesh::VTX_DECL_POSITION_COLOR, (3+4) * 4);

// ------------------------------------
D3D11_INPUT_ELEMENT_DESC layout_positions_normal_uv[] =
{
  { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
DECL_VERTEX_DECLARATION(positions_normal_uv, CMesh::VTX_DECL_POSITION_NORMAL_UV, (3 + 3 + 2) * 4);

// ---------------------------------
const CVertexDeclaration* CVertexDeclarationMgr::getByName( const std::string& name) {
  for (auto& v : all_decls) {
    if (v->name == name)
      return v;
  }
  fatal("Can't find vdecl named %s\n", name.c_str());
  return nullptr;
}

const CVertexDeclaration* CVertexDeclarationMgr::getById( uint32_t id ) {
  for (auto& v : all_decls) {
    if (v->id == id)
      return v;
  }
  fatal("Can't find vdecl with id %d\n", id);
  return nullptr;
}






