#ifndef INC_SKC_GUARD_H_
#define INC_SKC_GUARD_H_

#include "skel_controller.h"

class SkelControllerGuard : public TCompSkelController, public TCompBase {
protected:
	void myUpdate();

public:
	bool load(MKeyValue& atts) {
		FOOT_GROUND_EPSILON = 0.01f;
		FOOT_AIR_EPSILON = 0.015f;
		return true;
	}
	//void onCreate(const TMsgEntityCreated&);

	bool getUpdateInfo() override;
	void update(float elapsed) { TCompSkelController::update(); };

	void renderInMenu() { renderInMenuParent(); }

	SkelControllerGuard& SkelControllerGuard::operator=(SkelControllerGuard arg) { return arg; }
};

#endif
