#include "mcv_platform.h"
#include "resources/resource.h"
#include "vertex_declarations.h"
#include "mesh.h"
#include "imgui/imgui.h"

template<>
IResource::eType getTypeOfResource<CMesh>() {
  return IResource::MESH;
}

//DEFINE_RESOURCE(CMesh, MESH, "Mesh");

void CMesh::renderUIDebug() {
  ImGui::Text("# Vertexs: %d", num_vertexs);
  ImGui::Text("Bytes/Vertex: %d", num_bytes_per_vertex);
}

void CMesh::destroy() {
  SAFE_RELEASE(vb);
  SAFE_RELEASE(ib);
}

bool CMesh::create(
  uint32_t new_num_vertexs
, uint32_t new_num_bytes_per_vertex
, const void* initial_vertex_data
, uint32_t new_num_idxs
, uint32_t new_num_bytes_per_idx
, const void* initial_index_data
, eVertexDecl new_enum_vtx_decl
, ePrimitiveType new_topology
) {
  assert(vb == nullptr);
  assert(new_num_vertexs > 0);
  assert(new_num_bytes_per_vertex > 0);
  assert(initial_vertex_data != nullptr );

  // Translate the topology from our system to dx
  if( new_topology == TRIANGLE_LIST )
    topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  else if(new_topology == LINE_LIST)
    topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
  else {
    fatal("Unknown topology %d\n", new_topology);
  }

  // Translate the vtx decr from our system to dx11
  switch (new_enum_vtx_decl) {
  case VTX_DECL_POSITION_UV:
    assert(new_num_bytes_per_vertex == 4 * (3 + 2));
    vtx_decl = &vdecl_positions_uv; break;
  case VTX_DECL_POSITION_COLOR:
    assert(new_num_bytes_per_vertex == 4 * (3 + 4));
    vtx_decl = &vdecl_positions_color; break;
  default:
    fatal("Unknown vertex decl %d\n", new_enum_vtx_decl);
  }

  num_vertexs = new_num_vertexs;
  num_bytes_per_vertex = new_num_bytes_per_vertex;
  num_idxs = new_num_idxs;
  num_bytes_per_idx = new_num_bytes_per_idx;

  // Vertex buffer
  D3D11_BUFFER_DESC bd;
  memset(&bd, 0x00, sizeof(bd));
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = new_num_bytes_per_vertex * num_vertexs;
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA InitData;
  memset(&InitData, 0x00, sizeof(InitData));
  InitData.pSysMem = initial_vertex_data;
  HRESULT hr = Render.device->CreateBuffer(&bd, &InitData, &vb);
  if (FAILED(hr))
    return false;
  setDXName(vb, getName().c_str());

  // Index buffer
  if (num_idxs > 0) {
    assert(initial_index_data);
    assert(new_num_bytes_per_idx == 2 || new_num_bytes_per_idx == 4);
    memset(&bd, 0x00, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = new_num_bytes_per_idx * new_num_idxs;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    memset(&InitData, 0x00, sizeof(InitData));
    InitData.pSysMem = initial_index_data;
    hr = Render.device->CreateBuffer(&bd, &InitData, &ib);
    if (FAILED(hr))
      return false;

    setDXName(ib, getName().c_str());

  }

  return true;
}

void CMesh::activate() const {
  UINT stride = num_bytes_per_vertex;
  UINT offset = 0;
  Render.ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);

  // Set primitive topology
  Render.ctx->IASetPrimitiveTopology(topology);

  if (ib) {
    Render.ctx->IASetIndexBuffer( ib
      , (num_bytes_per_idx == 2)
      ? DXGI_FORMAT_R16_UINT
      : DXGI_FORMAT_R32_UINT
      , 0
      );
  }
}

void CMesh::render() const {
  if (ib)
    Render.ctx->DrawIndexed(num_idxs, 0, 0);
  else
    Render.ctx->Draw(num_vertexs, 0);
}

void CMesh::activateAndRender() const {
  activate();
  render();
}

