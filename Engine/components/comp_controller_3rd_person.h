#ifndef INC_COMPONENT_CONTROLLER_3RD_PERSON_H_
#define INC_COMPONENT_CONTROLLER_3RD_PERSON_H_

#include "comp_base.h"
#include "handle/handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_life.h"
#include "comp_msgs.h"
#include "geometry/angular.h"

struct TCompController3rdPerson : public TCompBase {
	CHandle target;
	float   yaw;
	float   pitch;
	float   distance_to_target;
	TCompController3rdPerson()
		: yaw(deg2rad(0.f))
		, pitch(deg2rad(0.f))
		, distance_to_target(5.0f)
	{}

	void onSetTarget(const TMsgSetTarget& msg) {
		target = msg.target;
	}

	void update(float dt) {
		CEntity* e_target = target;
		if (!e_target || ((TCompLife*)e_target->get<TCompLife>())->currentlife <= 0.0f)
			return;
		TCompTransform* target_tmx = e_target->get<TCompTransform>();
		assert(target_tmx);
		auto target_loc = target_tmx->getPosition();
		target_tmx->getAngles(&yaw, &pitch);
		VEC3 delta = getVectorFromYawPitch(yaw, pitch);
		auto origin = target_loc - delta * distance_to_target;

		CEntity* e_owner = CHandle(this).getOwner();
		TCompTransform* my_tmx = e_owner->get<TCompTransform>();
		my_tmx->lookAt(origin, target_loc);
	}
};

#endif
