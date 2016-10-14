#ifndef INC_COMPONENT_RENDER_STATIC_MESH_H_
#define INC_COMPONENT_RENDER_STATIC_MESH_H_

#include "comp_msgs.h"
#include "comp_base.h"

class CStaticMesh;

struct TCompRenderStaticMesh : public TCompBase {
	static CHandle last_loaded_static_mesh;
	bool onlyDebug = false;
	bool dynamic = false;
	const CStaticMesh* static_mesh;
	std::string res_name;
	~TCompRenderStaticMesh();
	void onCreate(const TMsgEntityCreated&);
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	void unregisterFromRender();
	void registerToRender();
	void onGetLocalAABB(const TMsgGetLocalAABB& msg);
	bool isDynamic() { return dynamic; }
};

#endif
