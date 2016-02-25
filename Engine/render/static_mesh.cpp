#include "mcv_platform.h"
#include "resources/resources_manager.h"
#include "texture.h"
#include "mesh.h"
#include "static_mesh.h"

template<> IResource::eType getTypeOfResource<CStaticMesh>() { return IResource::STATIC_MESH; }

template<>
IResource* createObjFromName<CStaticMesh>(const std::string& name) {
	CStaticMesh* obj = new CStaticMesh;
	if (!obj->load(name.c_str())) {
		dbg("Can't load CStaticMesh %s. Will try placeholder...\n", name.c_str());
		// try to load a placeholder...
		//if (!texture->load("missing.dds")) {
		//  fatal("Can't load texture %s\n", name.c_str());
		//}
	}
	obj->setName(name.c_str());
	return obj;
}

void CStaticMesh::onStartElement(const std::string &elem, MKeyValue &atts) {
	if (elem == "slot") {
		auto mat_name = atts["material"];
		auto mesh_name = atts["mesh"];
		TSlot s;
		s.mesh = Resources.get(mesh_name.c_str())->as<CMesh>();
		s.material = Resources.get(mat_name.c_str())->as<CMaterial>();
		s.submesh_idx = atts.getInt("submesh_idx", 0);
		slots.push_back(s);
	}
}

// ----------------------------------------------
bool CStaticMesh::load(const char* filename) {
	std::string full_path = getDataPath() + std::string(filename);
	bool is_ok = xmlParseFile(full_path);
	assert(is_ok);
	return true;
}