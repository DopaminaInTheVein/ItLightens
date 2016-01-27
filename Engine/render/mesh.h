#ifndef INC_RENDER_MESH_H_
#define INC_RENDER_MESH_H_

#include "render/render.h"
#include "resources/resource.h"

class CMesh : public IResource {

  ID3D11Buffer*           vb;

  uint32_t                num_vertexs;
  //uint32_t              num_indices;
  uint32_t                num_bytes_per_vertex;
  D3D_PRIMITIVE_TOPOLOGY  topology;

  std::string             name;

public:

  CMesh() : vb(nullptr) { }
  CMesh(const CMesh&) = delete;
  ~CMesh();

  void destroy();
  bool create(
      uint32_t new_num_vertexs
    , uint32_t new_num_bytes_per_vertex
    , const void* initial_vertex_data
    , D3D_PRIMITIVE_TOPOLOGY  new_topology
    );
  void activate() const;
  void render() const;
  void activateAndRender() const;

  const std::string& getName() const {
    return name;
  }
  bool isValid() const {
    return vb != nullptr;
  }

  eType getType() const { return MESH; }
  void renderUIDebug() override;

};

#endif

