#include "mcv_platform.h"
#include "comp_transform.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "utils/XMLParser.h"
#include "imgui/imgui.h"

#include "contants/ctes_object.h"

//provisional physics
#include "physics\simple_physx.h"
#include "physics\colliders.h"
#include "handle\handle.h"
#include "components\entity.h"

extern CSimplePhysx s_physx;

extern CShaderCte< TCteObject > shader_ctes_object;

void TCompTransform::render() const
{
	auto axis = Resources.get("axis.mesh")->as<CMesh>();
	if (getScale().x != 1) {
		VEC3 pos = getPosition();
		VEC3 posAux = pos / getScale().x;

		CTransform t;
		t.setPosition(posAux);
		t.setRotation(getRotation());
		t.setScale(getScale());
		shader_ctes_object.World = t.asMatrix();
	}
	else {
		shader_ctes_object.World = asMatrix();
	}
	shader_ctes_object.uploadToGPU();
	axis->activateAndRender();
}

bool TCompTransform::load(MKeyValue& atts) {
	auto p = atts.getPoint("pos");
	auto q = atts.getQuat("quat");
	auto s = atts.getFloat("scale", 1.0f);
	setPosition(p);
	setRotation(q);
	setScale(VEC3(s));
	return true;
}

void TCompTransform::renderInMenu() {
	VEC3 pos = getPosition();
	if (ImGui::SliderFloat3("Pos", &pos.x, -200.f, 200.f)) {
		setPosition(pos);
	}

	float yaw, pitch;
	getAngles(&yaw, &pitch);
	yaw = rad2deg(yaw);
	pitch = rad2deg(pitch);
	bool yaw_changed = ImGui::SliderFloat("Yaw", &yaw, -180.f, 180.f);
	bool pitch_changed = ImGui::SliderFloat("Pitch", &pitch, -90.f + 0.001f, 90.f - 0.001f);
	if (yaw_changed || pitch_changed)
		setAngles(deg2rad(yaw), deg2rad(pitch));

	float scale = getScale().x;
	if (ImGui::SliderFloat("Scale", &scale, 0.2f, 5.0f)) {
		setScale(VEC3(scale, scale, scale));
	}
}

bool TCompTransform::executeMovement(VEC3 new_pos)
{
	CHandle me = CHandle(this);
	CHandle owner = me.getOwner();
	CEntity *e = owner;

	VEC3 old_pos = getPosition();
	setPosition(new_pos);

	if (e->getCollisionType() == CEntity::SPHERE) {
		sphereCollider *sc = e->get<sphereCollider>();
		if (!s_physx.isMovementValid(owner, sc->getTag())) {
			setPosition(old_pos);
			return false;
		}
	}
	else if (e->getCollisionType() == CEntity::BOX) {
		boxCollider *bc = e->get<boxCollider>();
		if (!s_physx.isMovementValid(owner, bc->getTag())) {
			if (!s_physx.isCollidingBoxOverBox(owner, bc->getTag())) {
				setPosition(old_pos);
				return false;
			}
		}
	}

	return true;
}