#include "mcv_platform.h"
#include "comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/render_manager.h"
#include "render/static_mesh.h"
#include "components/comp_charactercontroller.h"
#include "components/entity.h"

ClHandle TCompRenderStaticMesh::last_loaded_static_mesh = ClHandle();

void TCompRenderStaticMesh::onCreate(const TMsgEntityCreated&) {
	registerToRender();
	last_loaded_static_mesh = ClHandle(this).getOwner();
}

bool TCompRenderStaticMesh::load(MKeyValue& atts) {
	res_name = atts["name"];
#ifndef NDEBUG
	if (res_name == "static_meshes/ms3/cable.static_mesh") {
		dbg("Cargando malla buscada...\n");
	}
#endif
	static_mesh = Resources.get(res_name.c_str())->as<CStaticMesh>();
	assert(static_mesh);
	return true;
}
bool TCompRenderStaticMesh::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("name", res_name);
	return true;
}

TCompRenderStaticMesh::~TCompRenderStaticMesh() {
	unregisterFromRender();
}

void TCompRenderStaticMesh::registerToRender() {
	RenderManager.registerToRender(static_mesh, ClHandle(this));
}

void TCompRenderStaticMesh::unregisterFromRender() {
	RenderManager.unregisterFromRender(ClHandle(this));
}

void TCompRenderStaticMesh::onGetLocalAABB(const TMsgGetLocalAABB& msg) {
	if (GETH_MY(TCompCharacterController).isValid()) return;
	AABB::CreateMerged(*msg.aabb, *msg.aabb, static_mesh->getAABB());
}