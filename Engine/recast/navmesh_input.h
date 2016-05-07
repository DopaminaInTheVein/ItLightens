#ifndef _NAVMESH_INPUT_INC
#define _NAVMESH_INPUT_INC

#include "mcv_platform.h"
#include <vector>

class CNavmeshInput {
public:
	struct TInput {
		VEC3 pmin;
		VEC3 pmax;
		TInput() : pmin(0, 0, 0), pmax(0, 0, 0) { }
	};
	static const int MAX_INPUTS = 1024;
	typedef std::vector< TInput > VInputs;
	typedef std::vector< PxTriangleMeshGeometry > VInputsTris;

public:
	VInputs               inputs;
	VInputsTris               inputsTris;
	VEC3           aabb_min;
	VEC3           aabb_max;

	float*                verts;
	int*                  tris;
	int                   nverts;
	int                   ntris;
	int                   nverts_total;
	int                   ntris_total;
	int                   nverts_total_mesh;
	int                   ntris_total_mesh;

public:
	CNavmeshInput();

	void clearInput();
	void addInput(const VEC3& p0, const VEC3& p1);
	void addInput(const PxTriangleMeshGeometry& t_mesh);
	void prepareInput(const TInput& input);
	void prepareInputMesh(const PxTriangleMeshGeometry& input);
	void unprepareInput();
	void computeBoundaries();
};

#endif
