#ifndef INC_COMPONENT_RENDER_STATIC_MESH_H_
#define INC_COMPONENT_RENDER_STATIC_MESH_H_

#include "comp_msgs.h"
#include "comp_base.h"

class CStaticMesh;

struct TCompRenderStaticMesh : public TCompBase {
	const CStaticMesh* static_mesh;
	~TCompRenderStaticMesh();
	void onCreate(const TMsgEntityCreated&);
	bool load(MKeyValue& atts);
	void unregisterFromRender();
	void registerToRender();
};

#endif

