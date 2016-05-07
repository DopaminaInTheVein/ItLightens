#include "mcv_platform.h"
#include "navmesh_input.h"
#include "components\comp_physics.h"
#include "Physx\inc\PxPhysics.h"
#include "PxPhysicsAPI.h"
#include <assert.h>
#include <map>
#include <string>

CNavmeshInput::CNavmeshInput()
	: verts(nullptr)
	, tris(nullptr)
	, nverts(0)
	, ntris(0)
	, nverts_total(0)
	, ntris_total(0)
	, nverts_total_mesh(0)
	, ntris_total_mesh(0)
{ }

// ---------------------------------------------------
void CNavmeshInput::clearInput() {
	inputs.clear();
	inputsTris.clear();
	nverts_total = 0;
	ntris_total = 0;
}

void CNavmeshInput::addInput(const VEC3& p0, const VEC3& p1) {
	//nverts_total += 8;
	//ntris_total += 10;

	TInput input;
	input.pmin = p0;
	input.pmax = p1;
	inputs.push_back(input);
}

void CNavmeshInput::addInput(const PxTriangleMeshGeometry& t_mesh) {
	ntris_total_mesh += t_mesh.triangleMesh->getNbTriangles();
	nverts_total_mesh += t_mesh.triangleMesh->getNbVertices();
	inputsTris.push_back(t_mesh);
}

// ---------------------------------------------------
/*void CNavmeshInput::prepareInput(const TInput& input) {
	unprepareInput();

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

	for (int i = 0; i < 8; ++i) {
		VEC3 p = v[i];
		int idx = i * 3;
		verts[idx] = p.x;
		verts[idx + 1] = p.y;
		verts[idx + 2] = p.z;
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
*/
void CNavmeshInput::prepareInputMesh(const PxTriangleMeshGeometry& input) {
	unprepareInput();

	nverts = input.triangleMesh->getNbVertices();
	ntris = input.triangleMesh->getNbTriangles();

	verts = new float[nverts * 3];
	tris = new int[ntris * 3];

	memset(verts, 0, nverts * 3 * sizeof(float));
	memset(tris, 0, ntris * 3 * sizeof(int));
	const PxVec3 * vertices = input.triangleMesh->getVertices();
	std::map <std::string, int> indexes;

	int inde = 0;
	for (int i = 0; i < nverts; ++i) {
		PxVec3 p = vertices[i];
		int idx = i * 3;
		verts[idx] = p.x;
		verts[idx + 1] = p.y;
		verts[idx + 2] = p.z;
		std::string key = std::to_string(p.x) + "/" + std::to_string(p.y) + "/" + std::to_string(p.z);
		if (indexes.find(key) == indexes.end()) {
			// not found
			indexes.insert(std::pair<std::string, int>(key, inde));
			inde++;
		}
	}
	const void * triangles = input.triangleMesh->getTriangles();

	int idx = 0;
	int I0, I1, I2;
	bool idxType = input.triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::eHAS_16BIT_TRIANGLE_INDICES;
	for (int tridx = 0; tridx < ntris; ++tridx) {
		if (idxType) {
			PxU16* P16BitIndices = (PxU16*)triangles;
			I0 = P16BitIndices[(tridx * 3) + 0];
			I1 = P16BitIndices[(tridx * 3) + 1];
			I2 = P16BitIndices[(tridx * 3) + 2];
		}
		else {
			PxU32* P32BitIndices = (PxU32*)triangles;
			I0 = P32BitIndices[(tridx * 3) + 0];
			I1 = P32BitIndices[(tridx * 3) + 1];
			I2 = P32BitIndices[(tridx * 3) + 2];
		}

		// Local position
		PxVec3  x = vertices[I0];
		PxVec3  y = vertices[I1];
		PxVec3  z = vertices[I2];

		std::string k1 = std::to_string(x.x) + "/" + std::to_string(x.y) + "/" + std::to_string(x.z);
		std::string k2 = std::to_string(y.x) + "/" + std::to_string(y.y) + "/" + std::to_string(y.z);
		std::string k3 = std::to_string(z.x) + "/" + std::to_string(z.y) + "/" + std::to_string(z.z);

		tris[idx++] = indexes.find(k1)->second;
		tris[idx++] = indexes.find(k2)->second;
		tris[idx++] = indexes.find(k3)->second;
	}

	assert(idx == ntris * 3);
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