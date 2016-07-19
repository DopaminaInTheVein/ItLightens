#include "mcv_platform.h"
#include "comp_skeleton_lookat.h"
#include "comp_skeleton.h"
#include "handle/handle.h"
#include "components/entity.h"
#include "components/comp_transform.h"

using namespace std;

bool TCompSkeletonLookAt::load(MKeyValue &atts) {
	target = atts.getPoint("target");
	amount = atts.getFloat("amount", 1.0f);
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

	// Access to the sibling comp skeleton component
	// where we can access the cal_model instance
	CalModel* model = my_skeleton->model;
	float angle = angleBetween(my_transform->getFront(), target - my_transform->getPosition());
	if (angle > max_angle) {
		return; //TODO desactivar lookat
	}
	//if (isKeyPressed('H')) {
	// Apply all the corrections of the core skeleton definition
	CCoreModel *core = (CCoreModel*)model->getCoreModel();
	for (auto bc : core->bone_corrections) {
		//bc.apply(model, Engine2Cal(target), amount * bc.local_amount);
	}
	//}
}

void TCompSkeletonLookAt::onCreate(const TMsgEntityCreated& msg)
{
	getUpdateInfo();
	CalModel* model = my_skeleton->model;
	CCoreModel *core = (CCoreModel*)model->getCoreModel();

	//Target
	target = my_transform->getPosition() + my_transform->getFront();

	for (int i = 0; i < look_at_bones.size(); i++) {
		CCoreModel::TBoneCorrector bone_corrector;
		auto bone_name = look_at_bones[i];
		bone_corrector.bone_id = my_skeleton->getKeyBoneId(bone_name);
		bone_corrector.local_amount = look_at_amoung[i];
		bone_corrector.local_dir = Engine2Cal(VEC3(0, 1, 0));
		core->bone_corrections.push_back(bone_corrector);
	}
}

void TCompSkeletonLookAt::renderInMenu()
{
	static float max_angle_deg = max_angle;
	if (ImGui::DragFloat("Max angle", &max_angle_deg), 1.f, 0.f, 360.f) {
		max_angle = deg2rad(max_angle_deg);
	}
	ImGui::Text("spine_down, spine_up, head, neck");
	if (ImGui::DragFloat4("Amounts", &look_at_amoung[0], 0.05f, 0.f, 1.f)) {
		CalModel* model = my_skeleton->model;
		CCoreModel *core = (CCoreModel*)model->getCoreModel();
		for (int i = 0; i < look_at_amoung.size(); i++) {
			core->bone_corrections[i].local_amount = look_at_amoung[i];
		}
	}
}