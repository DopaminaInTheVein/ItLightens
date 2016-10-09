#include "mcv_platform.h"
#include "navmesh_input.h"
#include "components\comp_physics.h"
#include <assert.h>

CNavmeshInput::CNavmeshInput()
  : verts(nullptr)
  , tris(nullptr)
  , nverts(0)
  , ntris(0)
  , nverts_total(0)
  , ntris_total(0)
{ }

// ---------------------------------------------------
void CNavmeshInput::clearInput() {
  inputs.clear();
  nverts_total = 0;
  ntris_total = 0;
}

void CNavmeshInput::addInput(const VEC3& p0, const VEC3& p1) {
  nverts_total += 8;
  ntris_total += 10;

  TInput input;
  input.pmin = p0;
  input.pmax = p1;
  inputs.push_back(input);
}

void CNavmeshInput::addInput(PxTriangleMesh* mesh, const VEC3& origin, const VEC3& p0, const VEC3& p1, const CQuaternion& rotation) {
  TInput input = TInput(mesh);
  input.rotation = rotation;

  auto verts = mesh->getNbVertices();
  auto ntris = mesh->getNbTriangles();

  nverts_total += verts;
  ntris_total += ntris;

  input.origin = origin;
  input.pmin = p0;
  input.pmax = p1;
  inputs.push_back(input);
}

// ---------------------------------------------------
void CNavmeshInput::prepareInput(const TInput& input) {
  unprepareInput();

  if (!input.mesh) {
    nverts = 8;
    ntris = 10;

    verts = new float[nverts * 3];
    tris = new int[ntris * 3];

    memset(verts, 0, nverts * 3 * sizeof(float));
    memset(tris, 0, ntris * 3 * sizeof(int));

    VEC3 v[8] = {
      VEC3(input.pmin.x, input.pmin.y, input.pmin.z)
      , VEC3(input.pmax.x, input.pmin.y, input.pmin.z)
      , VEC3(input.pmin.x, input.pmax.y, input.pmin.z)
      , VEC3(input.pmax.x, input.pmax.y, input.pmin.z)
      , VEC3(input.pmin.x, input.pmin.y, input.pmax.z)
      , VEC3(input.pmax.x, input.pmin.y, input.pmax.z)
      , VEC3(input.pmin.x, input.pmax.y, input.pmax.z)
      , VEC3(input.pmax.x, input.pmax.y, input.pmax.z)
    };

    static const int idxs[6][4] = {
      { 4, 6, 7, 5 }
      , { 5, 7, 3, 1 }
      , { 1, 3, 2, 0 }
      , { 0, 2, 6, 4 }
      , { 3, 7, 6, 2 }
      , { 5, 1, 0, 4 }
    };
    //  Debug->LogRaw("NEW INPUT NAVMESH\n");
    for (int i = 0; i < 8; ++i) {
      VEC3 p = v[i];
      int idx = i * 3;
      verts[idx] = p.x;
      verts[idx + 1] = p.y;
      verts[idx + 2] = p.z;

      // Debug->LogRaw("vtx point = %f, %f, %f\n", p.x, p.y, p.z);
    }

    int idx = 0;
    for (int i = 0; i < 5; ++i) {
      tris[idx++] = idxs[i][0];
      tris[idx++] = idxs[i][2];
      tris[idx++] = idxs[i][1];

      tris[idx++] = idxs[i][0];
      tris[idx++] = idxs[i][3];
      tris[idx++] = idxs[i][2];
    }

    assert(idx == ntris * 3);
  }
  else {
    auto mesh = input.mesh;

    nverts = mesh->getNbVertices();
    ntris = mesh->getNbTriangles();

    verts = new float[nverts * 3];
    tris = new int[ntris * 3];

    memset(verts, 0, nverts * 3 * sizeof(float));
    memset(tris, 0, ntris * 3 * sizeof(int));

    auto verticesPhysx = mesh->getVertices();
    //auto trisPhysx = mesh->getTriangles();
    //int* trisPhysxPointer = static_cast<int*>(trisPhysx);
    //Debug->LogRaw("NEW INPUT NAVMESH\n");
    for (int i = 0; i < nverts; ++i) {
      VEC3 p = PhysxConversion::PxVec3ToVec3(verticesPhysx[i]);

      // rotate
      VEC3 rp;
      rotate_vector_by_quaternion(p, input.rotation, rp);

      // traslate
      rp += input.origin;
      int idx = i * 3;
      verts[idx] = rp.x;
      verts[idx + 1] = rp.y;
      verts[idx + 2] = rp.z;

      //Debug->LogRaw("vtx point = %f, %f, %f\n", p.x, p.y, p.z);
    }

    auto trisPhysx = mesh->getTriangles();
    auto src = (const PxU16*)trisPhysx;

    /*for (PxU32 i = 0; i<ntris; i++)
    {
      indices[i * 3 + 0] = src[i * 3 + 0];
      indices[i * 3 + 1] = src[i * 3 + 2];
      indices[i * 3 + 2] = src[i * 3 + 1];
    }*/

    int idx = 0;
    for (int i = 0; i < ntris; i++) {
      tris[idx++] = (int)*src;
      // dbg("num = %d - ", (int)*src);
      src++;
      tris[idx++] = (int)*src;
      //dbg("%d - ", (int)*src);
      src++;
      tris[idx++] = (int)*src;
      // dbg("%d\n", (int)*src);
      src++;
    }

    assert(idx == ntris * 3);
  }
}

void CNavmeshInput::unprepareInput() {
  delete[] verts;
  delete[] tris;
  verts = 0;
  tris = 0;
}

void CNavmeshInput::computeBoundaries() {
  aabb_min = VEC3(0, 0, 0);
  aabb_max = VEC3(0, 0, 0);

  for (auto& i : inputs) {
    if (i.pmin.x < aabb_min.x)   aabb_min.x = i.pmin.x;
    if (i.pmin.y < aabb_min.y)   aabb_min.y = i.pmin.y;
    if (i.pmin.z < aabb_min.z)   aabb_min.z = i.pmin.z;
    if (i.pmax.x > aabb_max.x)   aabb_max.x = i.pmax.x;
    if (i.pmax.y > aabb_max.y)   aabb_max.y = i.pmax.y;
    if (i.pmax.z > aabb_max.z)   aabb_max.z = i.pmax.z;
  }
}