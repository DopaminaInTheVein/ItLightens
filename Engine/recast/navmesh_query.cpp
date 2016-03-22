#include "mcv_platform.h"
#include "navmesh_query.h"
#include <assert.h>
#include "recast/DebugUtils/Include/DetourDebugDraw.h"

void CNavmeshQuery::updatePosIni(VEC3& pos) {
	p1.p = pos;
	p1.set = true;
	if (data->m_navMesh)
		data->m_navQuery->findNearestPoly(&p1.p.x, m_polyPickExt, &m_filter, &m_startRef, 0);
}
void CNavmeshQuery::updatePosEnd(VEC3& pos) {
	p2.p = pos;
	p2.set = true;
	if (data->m_navMesh)
		data->m_navQuery->findNearestPoly(&p2.p.x, m_polyPickExt, &m_filter, &m_endRef, 0);
}

void CNavmeshQuery::render() {
	return;
}
// =============================================================== TOOLS
void CNavmeshQuery::setTool(ETool atool) {
	tool = atool;
	updateTool();
}

void CNavmeshQuery::resetTools() {
	m_startRef = 0;
	m_endRef = 0;
	m_npolys = 0;
	m_nstraightPath = 0;
	m_nsmoothPath = 0;
	memset(m_hitPos, 0, sizeof(m_hitPos));
	memset(m_hitNormal, 0, sizeof(m_hitNormal));
	m_distanceToWall = 0;

	m_filter.setIncludeFlags(0xffff);
	m_filter.setExcludeFlags(0);

	m_polyPickExt[0] = 2;
	m_polyPickExt[1] = 4;
	m_polyPickExt[2] = 2;

	m_neighbourhoodRadius = 2.5f;
	m_randomRadius = 5.0f;

	tool = FIND_PATH;
}

void CNavmeshQuery::updateTool() {
	// update the current tool
	if (tool == ETool::FIND_PATH)
		findPath(p1, p2);
	else if (tool == ETool::WALL_DISTANCE)
		wallDistance(p1);
	else if (tool == ETool::RAYCAST)
		raycast(p1, p2);
}

// --------------------------------------
// --------------------------------------
inline bool inRange(const float* v1, const float* v2, const float r, const float h) {
	const float dx = v2[0] - v1[0];
	const float dy = v2[1] - v1[1];
	const float dz = v2[2] - v1[2];
	return (dx*dx + dz*dz) < r*r && fabsf(dy) < h;
}

static int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
	const dtPolyRef* visited, const int nvisited) {
	int furthestPath = -1;
	int furthestVisited = -1;

	// Find furthest common polygon.
	for (int i = npath - 1; i >= 0; --i) {
		bool found = false;
		for (int j = nvisited - 1; j >= 0; --j) {
			if (path[i] == visited[j]) {
				furthestPath = i;
				furthestVisited = j;
				found = true;
			}
		}
		if (found)
			break;
	}

	// If no intersection found just return current path.
	if (furthestPath == -1 || furthestVisited == -1)
		return npath;

	// Concatenate paths.

	// Adjust beginning of the buffer to include the visited.
	const int req = nvisited - furthestVisited;
	const int orig = rcMin(furthestPath + 1, npath);
	int size = rcMax(0, npath - orig);
	if (req + size > maxPath)
		size = maxPath - req;
	if (size)
		memmove(path + req, path + orig, size*sizeof(dtPolyRef));

	// Store visited
	for (int i = 0; i < req; ++i)
		path[i] = visited[(nvisited - 1) - i];

	return req + size;
}

// This function checks if the path has a small U-turn, that is,
// a polygon further in the path is adjacent to the first polygon
// in the path. If that happens, a shortcut is taken.
// This can happen if the target (T) location is at tile boundary,
// and we're (S) approaching it parallel to the tile edge.
// The choice at the vertex can be arbitrary,
//  +---+---+
//  |:::|:::|
//  +-S-+-T-+
//  |:::|   | <-- the step can end up in here, resulting U-turn path.
//  +---+---+
static int fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery) {
	if (npath < 3)
		return npath;

	// Get connected polygons
	static const int maxNeis = 16;
	dtPolyRef neis[maxNeis];
	int nneis = 0;

	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
		return npath;

	for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next) {
		const dtLink* link = &tile->links[k];
		if (link->ref != 0) {
			if (nneis < maxNeis)
				neis[nneis++] = link->ref;
		}
	}

	// If any of the neighbour polygons is within the next few polygons
	// in the path, short cut to that polygon directly.
	static const int maxLookAhead = 6;
	int cut = 0;
	for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
		for (int j = 0; j < nneis; j++) {
			if (path[i] == neis[j]) {
				cut = i;
				break;
			}
		}
	}
	if (cut > 1) {
		int offset = cut - 1;
		npath -= offset;
		for (int i = 1; i < npath; i++)
			path[i] = path[i + offset];
	}

	return npath;
}

static bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
	const float minTargetDist,
	const dtPolyRef* path, const int pathSize,
	float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
	float* outPoints = 0, int* outPointCount = 0) {
	// Find steer target.
	static const int MAX_STEER_POINTS = 3;
	float steerPath[MAX_STEER_POINTS * 3];
	unsigned char steerPathFlags[MAX_STEER_POINTS];
	dtPolyRef steerPathPolys[MAX_STEER_POINTS];
	int nsteerPath = 0;
	navQuery->findStraightPath(startPos, endPos, path, pathSize,
		steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
	if (!nsteerPath)
		return false;

	if (outPoints && outPointCount) {
		*outPointCount = nsteerPath;
		for (int i = 0; i < nsteerPath; ++i)
			dtVcopy(&outPoints[i * 3], &steerPath[i * 3]);
	}

	// Find vertex far enough to steer to.
	int ns = 0;
	while (ns < nsteerPath) {
		// Stop at Off-Mesh link or when point is further than slop away.
		if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
			!inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
			break;
		ns++;
	}
	// Failed to find good point to steer to.
	if (ns >= nsteerPath)
		return false;

	dtVcopy(steerPos, &steerPath[ns * 3]);
	steerPos[1] = startPos[1];
	steerPosFlag = steerPathFlags[ns];
	steerPosRef = steerPathPolys[ns];

	return true;
}
// --------------------------------------
// --------------------------------------

void CNavmeshQuery::findPath(TPos& start, TPos& end) {
	m_pathFindStatus = DT_FAILURE;

	m_pathIterNum = 0;
	if (start.set && end.set && m_startRef && m_endRef) {
#ifdef DUMP_REQS
		printf("pi  %f %f %f  %f %f %f  0x%x 0x%x\n",
			m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
			m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif

		data->m_navQuery->findPath(m_startRef, m_endRef, &start.p.x, &end.p.x, &m_filter, m_polys, &m_npolys, MAX_POLYS);

		m_nsmoothPath = 0;

		if (m_npolys) {
			// Iterate over the path to find smooth path on the detail mesh surface.
			dtPolyRef polys[MAX_POLYS];
			memcpy(polys, m_polys, sizeof(dtPolyRef)*m_npolys);
			int npolys = m_npolys;

			float iterPos[3], targetPos[3];
			data->m_navQuery->closestPointOnPoly(m_startRef, &start.p.x, iterPos, 0);
			data->m_navQuery->closestPointOnPoly(polys[npolys - 1], &end.p.x, targetPos, 0);

			static const float STEP_SIZE = 0.5f;
			static const float SLOP = 0.01f;

			m_nsmoothPath = 0;

			dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
			m_nsmoothPath++;

			// Move towards target a small advancement at a time until target reached or
			// when ran out of memory to store the path.
			while (npolys && m_nsmoothPath < MAX_SMOOTH) {
				// Find location to steer towards.
				float steerPos[3];
				unsigned char steerPosFlag;
				dtPolyRef steerPosRef;

				if (!getSteerTarget(data->m_navQuery, iterPos, targetPos, SLOP,
					polys, npolys, steerPos, steerPosFlag, steerPosRef))
					break;

				bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
				bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

				// Find movement delta.
				float delta[3], len;
				dtVsub(delta, steerPos, iterPos);
				len = dtMathSqrtf(dtVdot(delta, delta));
				// If the steer target is end of path or off-mesh link, do not move past the location.
				if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
					len = 1;
				else
					len = STEP_SIZE / len;
				float moveTgt[3];
				dtVmad(moveTgt, iterPos, delta, len);

				// Move
				float result[3];
				dtPolyRef visited[16];
				int nvisited = 0;
				data->m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter,
					result, visited, &nvisited, 16);

				npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
				npolys = fixupShortcuts(polys, npolys, data->m_navQuery);

				float h = 0;
				data->m_navQuery->getPolyHeight(polys[0], result, &h);
				result[1] = h;
				dtVcopy(iterPos, result);

				// Handle end of path and off-mesh links when close enough.
				if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f)) {
					// Reached end of path.
					dtVcopy(iterPos, targetPos);
					if (m_nsmoothPath < MAX_SMOOTH) {
						dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
						m_nsmoothPath++;
					}
					break;
				}
				else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f)) {
					// Reached off-mesh connection.
					float startPos[3], endPos[3];

					// Advance the path up to and over the off-mesh connection.
					dtPolyRef prevRef = 0, polyRef = polys[0];
					int npos = 0;
					while (npos < npolys && polyRef != steerPosRef) {
						prevRef = polyRef;
						polyRef = polys[npos];
						npos++;
					}
					for (int i = npos; i < npolys; ++i)
						polys[i - npos] = polys[i];
					npolys -= npos;

					// Handle the connection.
					dtStatus status = data->m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
					if (dtStatusSucceed(status)) {
						if (m_nsmoothPath < MAX_SMOOTH) {
							dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
							m_nsmoothPath++;
							// Hack to make the dotted path not visible during off-mesh connection.
							if (m_nsmoothPath & 1) {
								dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
								m_nsmoothPath++;
							}
						}
						// Move position at the other side of the off-mesh link.
						dtVcopy(iterPos, endPos);
						float eh = 0.0f;
						data->m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
						iterPos[1] = eh;
					}
				}

				// Store results.
				if (m_nsmoothPath < MAX_SMOOTH) {
					dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
					m_nsmoothPath++;
				}
			}
		}
	}
	else {
		m_npolys = 0;
		m_nsmoothPath = 0;
	}
}

void CNavmeshQuery::wallDistance(TPos& pos) {
	m_distanceToWall = 0;
	if (pos.set && m_startRef) {
#ifdef DUMP_REQS
		printf("dw  %f %f %f  %f  0x%x 0x%x\n",
			m_spos[0], m_spos[1], m_spos[2], 100.0f,
			m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
		m_distanceToWall = 0.0f;
		data->m_navQuery->findDistanceToWall(m_startRef, &pos.p.x, 100.0f, &m_filter, &m_distanceToWall, m_hitPos, m_hitNormal);
	}
}

void CNavmeshQuery::raycast(TPos& start, TPos& end) {
	m_nstraightPath = 0;
	if (start.set && end.set && m_startRef) {
#ifdef DUMP_REQS
		printf("rc  %f %f %f  %f %f %f  0x%x 0x%x\n",
			m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
			m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
		float t = 0;
		m_npolys = 0;
		m_nstraightPath = 2;
		m_straightPath[0] = start.p.x;
		m_straightPath[1] = start.p.y;
		m_straightPath[2] = start.p.z;
		data->m_navQuery->raycast(m_startRef, &start.p.x, &end.p.x, &m_filter, &t, m_hitNormal, m_polys, &m_npolys, MAX_POLYS);
		if (t > 1) {
			// No hit
			dtVcopy(m_hitPos, &end.p.x);
			m_hitResult = false;
		}
		else {
			// Hit
			dtVlerp(m_hitPos, &start.p.x, &end.p.x, t);
			m_hitResult = true;
		}
		// Adjust height.
		if (m_npolys > 0) {
			float h = 0;
			data->m_navQuery->getPolyHeight(m_polys[m_npolys - 1], m_hitPos, &h);
			m_hitPos[1] = h;
		}
		dtVcopy(&m_straightPath[3], m_hitPos);
	}
}