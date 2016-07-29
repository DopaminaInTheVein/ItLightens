#include "mcv_platform.h"
#include "comp_skeleton_lookat.h"
#include "comp_skeleton.h"
#include "handle/handle.h"
#include "components/entity.h"
#include "components/comp_transform.h"

using namespace std;

bool TCompSkeletonLookAt::load(MKeyValue &atts) {
	target_prev = target = atts.getPoint("target");
	amount = atts.getFloat("amount", 1.0f);
	looking = false;
	return true;
}

bool TCompSkeletonLookAt::getUpdateInfo()
{
	my_transform = GETH_MY(TCompTransform);
	if (!my_transform) return false;
	my_skeleton = GETH_MY(TCompSkeleton);
	if (!my_skeleton) return false;

	return true;
}

void TCompSkeletonLookAt::update(float elapsed) {
	//Debug
	Debug->DrawLine(my_transform->getPosition(), target);

	// Recalc amount
	RecalcAmount();
	VEC3 new_target = RecalcTarget();

	if (amount <= 0.f) return;

	// Apply all the corrections of the core skeleton definition
	CalModel* model = my_skeleton->model;
	CCoreModel *core = (CCoreModel*)model->getCoreModel();
	for (auto bc : core->bone_corrections) {
		bc.apply(model, Engine2Cal(new_target), amount * bc.local_amount, max_angle_per_bone);
	}
}

void TCompSkeletonLookAt::RecalcAmount()
{
	bool increment = true;
	if (!looking) increment = false;
	else {
		float angle = angleBetween(my_transform->getFront(), target - my_transform->getPosition());
		if (angle > max_angle) {
			increment = false;
		}
	}

	if (increment) amount += getDeltaTime() * speed_transition;
	else amount -= getDeltaTime() * speed_transition;
	clamp_me(amount, 0.f, 1.f);
}

VEC3 TCompSkeletonLookAt::RecalcTarget()
{
	VEC3 res = target;
	if (isZero(target)) res = target_prev;
	//TODO? : amount_new_target to smooth change of targets

	return res;
}

void TCompSkeletonLookAt::onCreate(const TMsgEntityCreated& msg)
{
	getUpdateInfo();
	CalModel* model = my_skeleton->model;
	CCoreModel *core = (CCoreModel*)model->getCoreModel();

	//Target
	target = VEC3();
	amount = 0;

	for (int i = 0; i < look_at_bones.size(); i++) {
		CCoreModel::TBoneCorrector bone_corrector;
		auto bone_name = look_at_bones[i];
		bone_corrector.bone_id = my_skeleton->getKeyBoneId(bone_name);
		bone_corrector.local_amount = look_at_amount[i];
		bone_corrector.local_dir = Engine2Cal(VEC3(0, 1, 0));
		core->bone_corrections.push_back(bone_corrector);
	}
}

void TCompSkeletonLookAt::setTarget(const VEC3& new_target)
{
	if (target == new_target) return;
	target_prev = target;
	target = new_target;
	looking = !isZero(new_target);
}

void TCompSkeletonLookAt::renderInMenu()
{
	static float max_angle_deg = rad2deg(max_angle);
	if (ImGui::DragFloat("Max angle", &max_angle_deg), 1.f, 0.f, 180.f) {
		max_angle = deg2rad(max_angle_deg);
	}
	static float max_angle_per_bone_deg = rad2deg(max_angle_per_bone);
	if (ImGui::DragFloat("Max angle bone", &max_angle_per_bone_deg), 1.f, 0.f, 180.f) {
		max_angle_per_bone = deg2rad(max_angle_per_bone_deg);
	}

	ImGui::Text("spine_down, spine_up, head, neck");
	if (ImGui::DragFloat4("Amounts", &look_at_amount[0], 0.05f, 0.f, 1.f)) {
		CalModel* model = my_skeleton->model;
		CCoreModel *core = (CCoreModel*)model->getCoreModel();
		for (int i = 0; i < look_at_amount.size(); i++) {
			core->bone_corrections[i].local_amount = look_at_amount[i];
		}
	}
	if (ImGui::DragFloat("Time Transition", &time_transition, 0.05f, 0.05f, 2.f)) {
		speed_transition = 1 / time_transition;
	}
}