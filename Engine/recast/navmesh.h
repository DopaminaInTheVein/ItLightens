#ifndef _NAVMESH_INC
#define _NAVMESH_INC

#include "mcv_platform.h"
#include <vector>
#include "recast/Recast/Include/Recast.h"
#include "recast/Detour/Include/DetourNavMesh.h"
#include "recast/Detour/Include/DetourNavMeshQuery.h"
#include "navmesh_builder.h"
#include "navmesh_input.h"

class CNavmesh {
private:
	rcHeightfield*        m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet*         m_cset;//
	rcPolyMesh*           m_pmesh;//
	rcConfig              m_cfg;//
	rcPolyMeshDetail*     m_dmesh;//
	rcBuildContext*       m_ctx;//
	unsigned char*        m_triareas;//

	rcBuildContext        m_context;//

	rcConfig getRcConfig();
	void storeExtraData(std::string path);
	void restoreExtraData(std::string path);
public:
	enum {
		FLAG_WALK = 0x01
		, FLAG_SWIM = 0x02
		, FLAG_DISABLED = 0x10
		, ALL_FLAGS = 0xffff
	};

	enum EDrawMode {
		NAVMESH_DRAW_NONE = 0
		, NAVMESH_DRAW_TRANS
		, NAVMESH_DRAW_BVTREE
		, NAVMESH_DRAW_NODES
		, NAVMESH_DRAW_INVIS
		, NAVMESH_DRAW_MESH
		//, NAVMESH_DRAW_VOXELS
		//, NAVMESH_DRAW_VOXELS_WALKABLE
		//, NAVMESH_DRAW_COMPACT
		//, NAVMESH_DRAW_COMPACT_DISTANCE
		//, NAVMESH_DRAW_COMPACT_REGIONS
		//, NAVMESH_DRAW_REGION_CONNECTIONS
		//, NAVMESH_DRAW_RAW_CONTOURS
		//, NAVMESH_DRAW_BOTH_CONTOURS
		, NAVMESH_DRAW_COUNTOURS
		, NAVMESH_DRAW_POLYMESH
		, NAVMESH_DRAW_POLYMESH_DETAILS
		, NAVMESH_DRAW_TYPE_COUNT
	};

	dtNavMesh*            m_navMesh;//
	dtNavMeshQuery*       m_navQuery;//
	CNavmeshInput         m_input;//
	EDrawMode             m_draw_mode;//

	CNavmesh();
	void build(std::string salaloc, std::string salalocExtra);
	bool reload(std::string salaloc, std::string salalocExtra);

	dtNavMesh* create(const rcConfig& cfg, std::string salaloc, std::string salalocExtra);
	void prepareQueries();
	void destroy();
	void dumpLog();
	//void render(bool use_z_test) {};
};

#endif