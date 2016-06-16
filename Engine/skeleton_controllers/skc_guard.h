#ifndef INC_SKC_GUARD_H_
#define INC_SKC_GUARD_H_

#include "skel_controller.h"

class SkelControllerGuard : public TCompSkelController, public TCompBase {
protected:
	void myUpdate();

public:
	bool load(MKeyValue& atts) { return true; }
	//void onCreate(const TMsgEntityCreated&);

	bool getUpdateInfo() override;
	void update(float elapsed) { TCompSkelController::update(); };

	void renderInMenu() {}

	SkelControllerGuard& SkelControllerGuard::operator=(SkelControllerGuard arg) { return arg; }
};

#endif
