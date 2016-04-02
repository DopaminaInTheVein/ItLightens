#include "mcv_platform.h"
#include "utils/XMLParser.h"
#include "resources/resources_manager.h"
#include "comp_skeleton.h"
#include "skeleton.h"
#include "cal3d/cal3d.h"
#include "render/draw_utils.h"

#include "imgui/imgui.h"

#pragma comment(lib, "cal3d.lib" )

bool TCompSkeleton::load(MKeyValue& atts) {
	std::string res_name = atts.getString("model", "");   // warrior.skeleton
	resource_skeleton = Resources.get(res_name.c_str())->as<CSkeleton>();
	auto non_const_skel = const_cast<CSkeleton*>(resource_skeleton);
	model = new CalModel(non_const_skel->getCoreModel());

	// Play the first animation as cycle
	model->getMixer()->blendCycle(0, 1.0f, 0.f);
	return true;
}

void TCompSkeleton::renderInMenu() {
	static int anim_id = 0;
	static float in_delay = 0.3f;
	static float out_delay = 0.3f;
	static bool auto_lock = false;
	ImGui::DragInt("Anim Id", &anim_id, 0.1f, 0, model->getCoreModel()->getCoreAnimationCount());
	ImGui::DragFloat("In Delay", &in_delay, 0.1f, 0, 1.f);
	ImGui::DragFloat("Out Delay", &out_delay, 0.1f, 0, 1.f);
	ImGui::Checkbox("Auto lock", &auto_lock);
	if (ImGui::SmallButton("As Cycle")) {
		model->getMixer()->blendCycle(anim_id, 1.0f, in_delay);
	}
	if (ImGui::SmallButton("As Action")) {
		model->getMixer()->executeAction(anim_id, in_delay, out_delay, 1.0f, auto_lock);
	}

	auto mixer = model->getMixer();
	for (auto a : mixer->getAnimationActionList()) {
		ImGui::Text("Action %s S:%d W:%1.2f Time:%1.4f/%1.4f"
			, a->getCoreAnimation()->getName().c_str()
			, a->getState()
			, a->getWeight()
			, a->getTime()
			, a->getCoreAnimation()->getDuration()
			);
	}

	for (auto a : mixer->getAnimationCycle()) {
		ImGui::Text("Cycle %s S:%d W:%1.2f Time:%1.4f"
			, a->getCoreAnimation()->getName().c_str()
			, a->getState()
			, a->getWeight()
			, a->getCoreAnimation()->getDuration()
			);
	}
}

void TCompSkeleton::update(float dt) {
	model->update(getDeltaTime());
}

void TCompSkeleton::uploadBonesToCteShader() const {
	float* fout = &shader_ctes_bones.Bones[0]._11;

	CalSkeleton* skel = model->getSkeleton();
	auto& cal_bones = skel->getVectorBone();
	assert(cal_bones.size() < MAX_BONES_SUPPORTED);
	// For each bone from the cal model
	for (size_t bone_idx = 0; bone_idx < cal_bones.size(); ++bone_idx) {
		CalBone* bone = cal_bones[bone_idx];

		const CalMatrix& cal_mtx = bone->getTransformMatrix();
		const CalVector& cal_pos = bone->getTranslationBoneSpace();

		*fout++ = cal_mtx.dxdx;
		*fout++ = cal_mtx.dydx;
		*fout++ = cal_mtx.dzdx;
		*fout++ = 0.f;
		*fout++ = cal_mtx.dxdy;
		*fout++ = cal_mtx.dydy;
		*fout++ = cal_mtx.dzdy;
		*fout++ = 0.f;
		*fout++ = cal_mtx.dxdz;
		*fout++ = cal_mtx.dydz;
		*fout++ = cal_mtx.dzdz;
		*fout++ = 0.f;
		*fout++ = cal_pos.x;
		*fout++ = cal_pos.y;
		*fout++ = cal_pos.z;
		*fout++ = 1.f;
	}
	shader_ctes_bones.uploadToGPU();

	// Already done in game.cpp
	// shader_ctes_bones.activate(CTE_SHADER_BONES_SLOT);
}

void TCompSkeleton::render() const {
	auto skel = model->getSkeleton();
	size_t nbones = skel->getVectorBone().size();
	std::vector< VEC3 > bone_points;
	bone_points.resize(nbones * 2); // begin to end
	int nlines = skel->getBoneLines(&bone_points[0].x);
	float scale = 20.f;
	for (int i = 0; i < nlines; ++i) {
		Debug->DrawLine(bone_points[i * 2] * scale, bone_points[i * 2 + 1] * scale, VEC3(1, 0, 1));
	}
}