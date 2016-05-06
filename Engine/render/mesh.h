#ifndef INC_RENDER_MESH_H_
#define INC_RENDER_MESH_H_

#include "render/render.h"
#include "resources/resource.h"

struct CVertexDeclaration;

struct FileDataMesh {
	std::vector<unsigned char> vtxs;
	std::vector<unsigned char> idxs;
	uint32_t numVtx;
	uint32_t numIdx;
	uint32_t numVtxPerBytes;
	uint32_t numIdxPerBytes;
};

class CMesh : public IResource {
public:

  enum ePrimitiveType {
    TRIANGLE_LIST = 2000
  , TRIANGLE_STRIP
  , LINE_LIST
  };

  enum eVertexDecl {
    VTX_DECL_POSITION = 999
  , VTX_DECL_POSITION_UV = 1000
  , VTX_DECL_POSITION_COLOR 
  , VTX_DECL_POSITION_NORMAL_UV = 1002
  , VTX_DECL_POSITION_NORMAL_UV_TANGENT = 1003
  , VTX_DECL_POSITION_NORMAL_UV_SKIN = 1102
  , VTX_DECL_POSITION_NORMAL_UV_TANGENT_SKIN = 1103
  };

	struct TGroup {
		uint32_t first_index;
		uint32_t num_indices;
	};
	typedef std::vector<TGroup> VGroups;

	CMesh(const std::string& new_name)
		: vb(nullptr)
		, ib(nullptr)
		, vtx_decl(nullptr)
		, name(new_name)
	{ }
	CMesh(const CMesh&) = delete;

  void destroy();
  bool create(
    uint32_t new_num_vertexs
    , uint32_t new_num_bytes_per_vertex
    , const void* initial_vertex_data
    , uint32_t new_num_idxs
    , uint32_t new_num_bytes_per_index
    , const void* initial_index_data
    , eVertexDecl new_enum_vtx_decl
    , ePrimitiveType new_topology
    , const VGroups* groups = nullptr
    );
  void activate() const;
  void render() const;
  void renderGroup( uint32_t group_idx ) const;
  void activateAndRender() const;

	static FileDataMesh loadData(std::string path, CDataProvider & dp);
	AABB getAABB() const { return aabb; }
	uint32_t getNumVtxs() const {
		return num_vertexs;
	}

	uint32_t getNumIdxs() const {
		return num_idxs;
	}

	ID3D11Buffer* getVtxBuffer() const {
		return vb;
	}

	ID3D11Buffer* getIdxBuffer() const {
		return ib;
	}

	uint32_t getBytesPerVtx() const {
		return num_bytes_per_vertex;
	}

	uint32_t getBytesPerIdx() const {
		return num_bytes_per_idx;
	}

	const CVertexDeclaration* vtx_decl;

	const std::string& getName() const {
		return name;
	}
	bool isValid() const {
		return vb != nullptr;
	}

	eType getType() const { return MESH; }
	void renderUIDebug() override;

private:
	ID3D11Buffer*             vb;
	ID3D11Buffer*             ib;

	uint32_t                  num_vertexs;
	uint32_t                  num_idxs;
	uint32_t                  num_bytes_per_vertex;
	uint32_t                  num_bytes_per_idx;
	D3D_PRIMITIVE_TOPOLOGY    topology;

	std::string               name;
	VGroups                   groups;
	AABB                      aabb;

	static const CMesh* curr_mesh;
};

#endif
