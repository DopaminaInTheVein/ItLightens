#ifndef INC_RENDER_MESH_H_
#define INC_RENDER_MESH_H_

#include "render/render.h"
#include "resources/resource.h"

struct CVertexDeclaration;

class CMesh : public IResource {

public:

  enum ePrimitiveType {
    TRIANGLE_LIST = 2000
  , LINE_LIST
  };

  enum eVertexDecl {
    VTX_DECL_POSITION = 999
  , VTX_DECL_POSITION_UV = 1000
  , VTX_DECL_POSITION_COLOR 
  , VTX_DECL_POSITION_NORMAL_UV = 1002
  , VTX_DECL_POSITION_NORMAL_UV_SKIN = 1102
  };

  struct TGroup {
    uint32_t first_index;
    uint32_t num_indices;
  };
  typedef std::vector<TGroup> VGroups;

  CMesh( const std::string& new_name) 
    : vb(nullptr)
    , ib(nullptr)
    , vtx_decl(nullptr)
    , name( new_name )
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

  const CVertexDeclaration* vtx_decl;

  std::string               name;
  VGroups                   groups;

};

#endif

