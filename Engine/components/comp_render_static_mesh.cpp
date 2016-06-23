#include "mcv_platform.h"
#include "comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/render_manager.h"
#include "render/static_mesh.h"

void TCompRenderStaticMesh::onCreate(const TMsgEntityCreated&) {
	registerToRender();
}

bool TCompRenderStaticMesh::load(MKeyValue& atts) {
	auto res_name = atts["name"];
	static_mesh = Resources.get(res_name.c_str())->as<CStaticMesh>();
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
	if (static_mesh)
		AABB::CreateMerged(*msg.aabb, *msg.aabb, static_mesh->getAABB());
}