#include "mcv_platform.h"
#include "comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/render_manager.h"
#include "render/static_mesh.h"
#include "components/comp_charactercontroller.h"
#include "components/entity.h"

void TCompRenderStaticMesh::onCreate(const TMsgEntityCreated&) {
	registerToRender();
}

bool TCompRenderStaticMesh::load(MKeyValue& atts) {
	auto res_name = atts["name"];
#ifndef NDEBUG
	if (res_name == "static_meshes/obj/box2.static_mesh") {
		dbg("Cargando malla buscada...\n");
	}
#endif
	static_mesh = Resources.get(res_name.c_str())->as<CStaticMesh>();
	assert(static_mesh);
	return true;
}

TCompRenderStaticMesh::~TCompRenderStaticMesh() {
	unregisterFromRender();
}

void TCompRenderStaticMesh::registerToRender() {
	RenderManager.registerToRender(static_mesh, CHandle(this));
}

void TCompRenderStaticMesh::unregisterFromRender() {
	RenderManager.unregisterFromRender(CHandle(this));
}

void TCompRenderStaticMesh::onGetLocalAABB(const TMsgGetLocalAABB& msg) {
	if (GETH_MY(TCompCharacterController).isValid()) return;
	AABB::CreateMerged(*msg.aabb, *msg.aabb, static_mesh->getAABB());
}