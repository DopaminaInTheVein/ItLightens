#include "mcv_platform.h"
#include "comp_skeleton_lookat.h"
#include "comp_skeleton.h"
#include "handle/handle.h"
#include "components/entity.h"

bool TCompSkeletonLookAt::load(MKeyValue &atts) {
	target = atts.getPoint("target");
	amount = atts.getFloat("amount", 1.0f);
	return true;
}

void TCompSkeletonLookAt::update(float elapsed) {
	// Access to the sibling comp skeleton component
	// where we can access the cal_model instance
	CEntity* e = CHandle(this).getOwner();
	TCompSkeleton *comp_skel = e->get<TCompSkeleton>();
	if (comp_skel == nullptr)
		return;

	CalModel* model = comp_skel->model;

	if (isKeyPressed('H')) {
		// Apply all the corrections of the core skeleton definition
		CCoreModel *core = (CCoreModel*)model->getCoreModel();
		for (auto bc : core->bone_corrections) {
			bc.apply(model, Engine2Cal(target), amount * bc.local_amount);
		}
	}
}

void TCompSkeletonLookAt::onCreate(const TMsgEntityCreated& msg)
{
	CEntity* e = CHandle(this).getOwner();
	TCompSkeleton *comp_skel = e->get<TCompSkeleton>();
	if (comp_skel == nullptr)
		return;

	CalModel* model = comp_skel->model;
	CCoreModel *core = (CCoreModel*)model->getCoreModel();

	//TODO: Cargar head, neck, spine_up, spine_down y pelvis
}