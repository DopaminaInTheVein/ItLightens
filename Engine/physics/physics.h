#pragma once

#ifndef _INC_PHYSYCS_
#define _INC_PHYSICS_

#include "geometry/geometry.h"
#include "components/entity.h"

#define COL_TAG_PLAYER 1
#define COL_TAG_ENEMY 1 << 1
#define COL_TAG_OBJECT 1 << 2

struct ray_cast_result {
	CHandle firstCollider;
	VEC3 positionCollision;
	ray_cast_result() : firstCollider(CHandle()), positionCollision(VEC3(0, 0, 0)) {};
};

struct ray_cast_query {
	float maxDistance;
	VEC3 position;
	VEC3 direction;
	char types;
	ray_cast_query(VEC3 pos, VEC3 dir, float dist, char t) :
		maxDistance(dist)
		, position(pos)
		, direction(dir)
		, types(t) {};
	ray_cast_query() :
		maxDistance(0)
		, position(VEC3(0, 0, 0))
		, direction(VEC3(0, 0, 0))
		, types(0) {};
};

struct ray_cast_halfway {
	CHandle handle;
	ray_cast_query query;
	VEC3 posCollision;
	ray_cast_halfway(ray_cast_query rcQuery) :
		handle(CHandle())
		, posCollision(VEC3(0, 0, 0))
		, query(rcQuery) {};

	ray_cast_halfway() :
		handle(CHandle())
		, posCollision(VEC3(0, 0, 0))
		, query(ray_cast_query()) {};
};

class Physics {
public:
	static ray_cast_halfway RayCastHalfWay;
	static ray_cast_result calcRayCast(const ray_cast_query&);
};

#endif