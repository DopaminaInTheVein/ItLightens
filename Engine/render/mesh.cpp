#include "mcv_platform.h"
#include "resources/resource.h"
#include "vertex_declarations.h"
#include "mesh.h"
#include "render/mesh_format.h"
#include "imgui/imgui.h"

using namespace MeshFormat;

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
	, const CMesh::VGroups* new_groups
	) {
	assert(vb == nullptr);
	assert(new_num_vertexs > 0);
	assert(new_num_bytes_per_vertex > 0);
	assert(initial_vertex_data != nullptr);

  // Translate the topology from our system to dx
  if( new_topology == TRIANGLE_LIST )
    topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  else if (new_topology == TRIANGLE_STRIP)
    topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
  else if (new_topology == LINE_LIST)
    topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
  else {
    fatal("Unknown topology %d\n", new_topology);
  }

	// Translate the vtx decr from our system to dx11
	vtx_decl = vdecl_manager.getById(new_enum_vtx_decl);
	assert(vtx_decl->bytes_per_vertex == new_num_bytes_per_vertex);

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

	// Upgrade group info
	if (new_groups) {
		groups = *new_groups;
	}
	else {
		// Generate a single fake group
		groups.resize(1);
		groups[0].first_index = 0;
		if (num_idxs > 0)    // If the mesh is indexed
			groups[0].num_indices = num_idxs;
		else
			groups[0].num_indices = num_vertexs;
	}

	return true;
}

void CMesh::activate() const {
	PROFILE_FUNCTION("render mesh: activate");
	UINT stride = num_bytes_per_vertex;
	UINT offset = 0;
	Render.ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);

	// Set primitive topology
	Render.ctx->IASetPrimitiveTopology(topology);

	if (ib) {
		Render.ctx->IASetIndexBuffer(ib
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

void CMesh::renderGroup(uint32_t group_idx) const {
  assert(group_idx < (uint32_t)groups.size());
  const auto& g = groups[group_idx];
  if (ib)
    Render.ctx->DrawIndexed(g.num_indices, g.first_index, 0);
  else
    Render.ctx->Draw(g.num_indices, g.first_index);
}

void CMesh::activateAndRender() const {
	activate();
	render();
}

FileDataMesh CMesh::loadData(std::string path, CDataProvider& dp)
{
	FileDataMesh dm;

	struct TRiff {
		uint32_t magic;
		uint32_t num_bytes;
	};

	typedef std::vector< unsigned char > TBuffer;
	THeader header;
	TBuffer vtxs;
	TBuffer idxs;

	bool end_found = false;
	while (!end_found) {
		TRiff riff;
		dp.read(riff);

		switch (riff.magic) {
		case magic_header:    // Magic header
			dp.read(header);
			assert(header.isValid());
			break;

		case magic_vtxs:
			assert(riff.num_bytes == header.bytes_per_vtx * header.num_vtxs);
			vtxs.resize(riff.num_bytes);
			dp.readBytes(&vtxs[0], riff.num_bytes);
			break;

		case magic_idxs:
			assert(riff.num_bytes == header.bytes_per_idx * header.num_idxs);
			idxs.resize(riff.num_bytes);
			dp.readBytes(&idxs[0], riff.num_bytes);
			break;

		case magic_mesh_end:
			end_found = true;
			break;

		default:
			fatal("Unknown riff code %08x reading mesh\n", riff.magic);
			break;
		}
	}

	dm.idxs = idxs;
	dm.numIdxPerBytes = header.bytes_per_idx;
	dm.vtxs = vtxs;
	dm.numVtxPerBytes = header.bytes_per_vtx;
	dm.numIdx = header.num_idxs;
	dm.numVtx = header.num_vtxs;

	return dm;
}