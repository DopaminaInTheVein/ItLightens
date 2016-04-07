#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/mesh_format.h"

using namespace MeshFormat;

bool meshLoader(CMesh* mesh, CDataProvider& dp) {

  assert(dp.isValid());

  typedef std::vector< unsigned char > TBuffer;
  THeader header;
  TBuffer vtxs;
  TBuffer idxs;
  CMesh::VGroups groups;

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

    case magic_groups:
      assert(riff.num_bytes == header.num_groups * sizeof(CMesh::TGroup));
      assert(header.num_groups > 0);
      groups.resize(header.num_groups);
      dp.readBytes(&groups[0], riff.num_bytes);
      break;

    case magic_mesh_end:
      end_found = true;
      break;

    default:
      fatal("Unknown riff code %08x reading mesh\n", riff.magic);
      break;
    }

  }

  // 


  return mesh->create(
    header.num_vtxs
  , header.bytes_per_vtx
  , &vtxs[0]
  , header.num_idxs
  , header.bytes_per_idx
  , &idxs[0]
  , (CMesh::eVertexDecl)header.vertex_type
  , (CMesh::ePrimitiveType) header.primitive_type
  , &groups
  );

}








// --------------------------------
struct SimpleVertexColored
{
  float x, y, z;
  float r, g, b, a;
  void set(VEC3 npos, VEC4 color) {
    x = npos.x;
    y = npos.y;
    z = npos.z;
    r = color.x;
    g = color.y;
    b = color.z;
    a = color.w;
  }
};

bool createGridXZ(CMesh& mesh, int nsteps) {
  assert(nsteps > 0);
  std::vector< SimpleVertexColored > vtxs;
  // Two halfs + zero, two sides, two axis
  size_t nvtxs = (nsteps * 2 + 1) * 2 * 2;
  vtxs.resize(nvtxs);
  float g1 = 1.0f;
  float g2 = 0.2f;
  VEC4 c1(g1, g1, g1, 1);
  VEC4 c2(g2, g2, g2, 1);
  int k = 0;
  for (int i = -nsteps; i <= nsteps; ++i) {
    // To avoid warnings
    float fsteps = static_cast<float>(nsteps);
    float fit = static_cast<float>(i);
    VEC4 color = ((i % 5) == 0) ? c1 : c2;
    vtxs[k].set(VEC3(fsteps, 0, fit), color); ++k;
    vtxs[k].set(VEC3(-fsteps, 0, fit), color); ++k;
    vtxs[k].set(VEC3(fit, 0, fsteps), color); ++k;
    vtxs[k].set(VEC3(fit, 0, -fsteps), color); ++k;
  }
  assert(k == nvtxs);
  return mesh.create(
    (uint32_t)nvtxs,
    (uint32_t)sizeof(SimpleVertexColored), 
    &vtxs[0], 
    0, 0, nullptr,
    CMesh::VTX_DECL_POSITION_COLOR,
    CMesh::LINE_LIST
    );
}




// ------------------------------------------
template<>
IResource* createObjFromName<CMesh>(const std::string& name) {

  CMesh* mesh = new CMesh(name);

  // ----------------------------------
  if (name == "axis.mesh") {
    SimpleVertexColored vtxs_axis[6] =
    {
      { 0.0f, 0.0f, 0.0f,    1, 0, 0, 1 },    // X+
      { 1.0f, 0.0f, 0.0f,    1, 0, 0, 1 },
      { 0.0f, 0.0f, 0.0f,    0, 1, 0, 1 },    // Y+ x2
      { 0.0f, 2.0f, 0.0f,    0, 1, 0, 1 },
      { 0.0f, 0.0f, 0.0f,    0, 0, 1, 1 },    // Z+ x3
      { 0.0f, 0.0f, 3.0f,    0, 0, 1, 1 },
    };
    if (!mesh->create(6
      , sizeof(SimpleVertexColored)
      , vtxs_axis
      , 0, 0, nullptr
      , CMesh::VTX_DECL_POSITION_COLOR
      , CMesh::LINE_LIST))
      return nullptr;
    return mesh;
  }
  // ----------------------------------
  else if (name == "line.mesh") {
    SimpleVertexColored vtxs[2] =
    {
      { 0.0f, 0.0f, 0.0f,    1, 1, 1, 1 },    // X+
      { 0.0f, 0.0f, 1.0f,    1, 1, 1, 1 },
    };
    if (!mesh->create(2
      , sizeof(SimpleVertexColored)
      , vtxs
      , 0, 0, nullptr
      , CMesh::VTX_DECL_POSITION_COLOR
      , CMesh::LINE_LIST))
      return nullptr;
    return mesh;
  }
  // ----------------------------------
  else if (name == "frustum.mesh") {
    float zfar = 1.f;
    float znear = 0.f;
    SimpleVertexColored vtxs[8] =
    {
      { 1.0f, 1.0f, zfar,    1, 1, 1, 1 },    // 
      { 1.0f,-1.0f, zfar,    1, 1, 1, 1 },
      { -1.0f,-1.0f, zfar,   1, 1, 1, 1 },    // 
      { -1.0f, 1.0f, zfar,   1, 1, 1, 1 },
      { 1.0f, 1.0f, znear,   1, 1, 1, 1 },    // 
      { 1.0f,-1.0f, znear,   1, 1, 1, 1 },
      { -1.0f,-1.0f, znear,  1, 1, 1, 1 },    // 
      { -1.0f, 1.0f, znear,  1, 1, 1, 1 },
    };
    std::vector<uint16_t> idxs;
    for (int i = 0; i < 4; ++i) {
      idxs.push_back(i);
      idxs.push_back((i + 1) % 4);
      idxs.push_back(4 + i);
      idxs.push_back(4 + ((i + 1) % 4));
      idxs.push_back(i);
      idxs.push_back(i+4);
    }

    if (!mesh->create(8
      , sizeof(SimpleVertexColored)
      , vtxs
      , (uint32_t) idxs.size(), 2, &idxs[0] 
      , CMesh::VTX_DECL_POSITION_COLOR
      , CMesh::LINE_LIST))
      return nullptr;
    return mesh;
  }
  // ----------------------------------
  else if (name == "grid.mesh") {
    if (!createGridXZ(*mesh, 10))
      return nullptr;
    return mesh;
  }

  // ----------------------------------
  // Try to load from disk
  std::string full_path = IResource::getDataPath() + name;
  CFileDataProvider fdp(full_path.c_str());
  assert(fdp.isValid() || fatal("Can't open mesh file %s\n", full_path.c_str()));
  bool is_ok = meshLoader(mesh, fdp);
  assert(is_ok);
  return mesh;
}

