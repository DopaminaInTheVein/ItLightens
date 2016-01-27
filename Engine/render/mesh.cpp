#include "mcv_platform.h"
#include "resources/resource.h"
#include "mesh.h"
#include "imgui/imgui.h"

template<>
IResource::eType getTypeOfResource<CMesh>() {
  return IResource::MESH;
}

void CMesh::renderUIDebug() {
  ImGui::Text("# Vertexs: %d", num_vertexs);
  ImGui::Text("Bytes/Vertex: %d", num_bytes_per_vertex);
}



CMesh::~CMesh() {
  destroy();
}

void CMesh::destroy() {
  if (vb)
    vb->Release(), vb = nullptr;
}

bool CMesh::create(
  uint32_t new_num_vertexs
  , uint32_t new_num_bytes_per_vertex
  , const void* initial_vertex_data
  , D3D_PRIMITIVE_TOPOLOGY new_topology
  ) {
  assert(vb == nullptr);
  assert(new_num_vertexs > 0);
  assert(new_num_bytes_per_vertex > 0);
  assert(initial_vertex_data != nullptr );

  topology = new_topology;
  num_vertexs = new_num_vertexs;
  num_bytes_per_vertex = new_num_bytes_per_vertex;

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

  return true;
}

void CMesh::activate() const {
  UINT stride = num_bytes_per_vertex;
  UINT offset = 0;
  Render.ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);

  // Set primitive topology
  Render.ctx->IASetPrimitiveTopology(topology);
}

void CMesh::render() const {
  Render.ctx->Draw(num_vertexs, 0);
}

void CMesh::activateAndRender() const {
  activate();
  render();
}

