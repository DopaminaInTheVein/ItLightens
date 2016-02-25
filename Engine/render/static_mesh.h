#ifndef INC_RENDER_STATIC_MESH_H_
#define INC_RENDER_STATIC_MESH_H_

#include "utils/XMLParser.h"
#include "resources/resource.h"

class CMaterial;
class CMesh;

class CStaticMesh : public IResource, public CXMLParser {
	void onStartElement(const std::string &elem, MKeyValue &atts);

public:

	struct TSlot {
		const CMesh*     mesh;
		int              submesh_idx;
		const CMaterial* material;
	};
	std::vector<TSlot > slots;

	bool isValid() const override { return true; }
	void destroy() { }
	eType getType() const override { return STATIC_MESH; }

	//void renderUIDebug();
	bool load(const char* filename);
};

#endif
