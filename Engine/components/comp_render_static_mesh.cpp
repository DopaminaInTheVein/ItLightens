#include "mcv_platform.h"
#include "comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/render_manager.h"
#include "render/static_mesh.h"
#include "components/comp_charactercontroller.h"
#include "components/entity.h"

CHandle TCompRenderStaticMesh::last_loaded_static_mesh = CHandle();

void TCompRenderStaticMesh::onCreate(const TMsgEntityCreated&) {
	registerToRender();
	last_loaded_static_mesh = CHandle(this).getOwner();
}

bool TCompRenderStaticMesh::load(MKeyValue& atts) {
	res_name = atts["name"];
	static_mesh = Resources.get(res_name.c_str())->as<CStaticMesh>();
	assert(static_mesh);

	dynamic = !atts.getBool("static", true);
	to_render = atts.getBool("to_render", true);
	cast_shadows = atts.getBool("cast_shadows", true);
	if (!cast_shadows) {
		cast_shadows_dyn = false;
		auto type_cast_shadows = atts.getString("cast_shadows", "");
		if (type_cast_shadows != "static") {
			cast_shadows = false;
		}
	}
	return true;
}
bool TCompRenderStaticMesh::save(std::ofstream& os, MKeyValue& atts)
{
	atts.put("name", res_name);
	atts.put("static", !dynamic);
	if (!to_render) atts.put("to_render", to_render);
	return true;
}

TCompRenderStaticMesh::~TCompRenderStaticMesh() {
	unregisterFromRender();
}

void TCompRenderStaticMesh::registerToRender() {
	if (to_render)
		RenderManager.registerToRender(static_mesh, CHandle(this));
}

void TCompRenderStaticMesh::unregisterFromRender() {
	RenderManager.unregisterFromRender(CHandle(this));
}

void TCompRenderStaticMesh::onGetLocalAABB(const TMsgGetLocalAABB& msg) {
	if (GETH_MY(TCompCharacterController).isValid()) return;
	AABB::CreateMerged(*msg.aabb, *msg.aabb, static_mesh->getAABB());
}
void TCompRenderStaticMesh::renderInMenu()
{
	ImGui::Text(res_name.c_str());
}