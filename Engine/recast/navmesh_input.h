#ifndef _NAVMESH_INPUT_INC
#define _NAVMESH_INPUT_INC

#include "mcv_platform.h"
#include <vector>

class CNavmeshInput {
public:
  struct TInput {
    VEC3 pmin;
    VEC3 pmax;
    VEC3 origin;
    CQuaternion rotation;
    PxTriangleMesh* mesh;
    TInput(PxTriangleMesh* i_mesh = nullptr) : pmin(0, 0, 0), pmax(0, 0, 0), mesh(i_mesh) { }
  };
  static const int MAX_INPUTS = 1024;
  typedef std::vector< TInput > VInputs;

public:
  VInputs         inputs;
  VEC3           aabb_min;
  VEC3           aabb_max;

  float*                verts;
  int*                  tris;
  int                   nverts;
  int                   ntris;
  int                   nverts_total;
  int                   ntris_total;

public:
  CNavmeshInput();

  void clearInput();
  void addInput(const VEC3& p0, const VEC3& p1);
  void addInput(PxTriangleMesh * mesh, const VEC3& origin, const VEC3& p0, const VEC3& p1, const CQuaternion& rotat);
  void prepareInput(const TInput& input);
  void unprepareInput();
  void computeBoundaries();
};

#endif
