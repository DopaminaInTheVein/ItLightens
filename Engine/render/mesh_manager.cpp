#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"

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
  return mesh.create(nvtxs, sizeof(SimpleVertexColored), &vtxs[0], D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

// ------------------------------------------
template<>
IResource* createObjFromName<CMesh>(const std::string& name) {

  CMesh* mesh = new CMesh;

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
    if (!mesh->create(6, sizeof(SimpleVertexColored), vtxs_axis, D3D_PRIMITIVE_TOPOLOGY_LINELIST))
      return nullptr;
    return mesh;
  }
  // ----------------------------------
  else if (name == "grid.mesh") {
    if (!createGridXZ(*mesh, 10))
      return nullptr;
    return mesh;
  }

  fatal("Need to implement...");
  return nullptr;
}

