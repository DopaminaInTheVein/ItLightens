#ifndef INC_COMPONENT_CONTROLLER_3RD_PERSON_H_
#define INC_COMPONENT_CONTROLLER_3RD_PERSON_H_

#include "comp_base.h"
#include "handle/handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_msgs.h"
#include "geometry/angular.h"

#include "input\input.h"

extern CInput Input;

class TCompController3rdPerson : public TCompBase {
	CHandle		target;
	float		yaw;
	float		pitch;
	float		distance_to_target;
	VEC3		position_diff;
	float		speed_camera;
	float		m_yaw;
	float		m_pitch;
	float		min_pitch = -20.0f;
	float		max_pitch = 75.0f;


public:
	TCompController3rdPerson()
		: yaw(deg2rad(0.f))
		, pitch(deg2rad(0.f))
		, distance_to_target(5.0f)
		, position_diff(VEC3(0, 0, 0))
		, speed_camera(20)
		,m_yaw(0.0f)
		,m_pitch(0.0f)
	{}


	void onSetTarget(const TMsgSetTarget& msg) {
		target = msg.target;
	}

	void updateInput() {
		

		if (Input.GetMouseDiffX() != 0) {
			m_yaw -= Input.GetMouseDiffX()*speed_camera*getDeltaTime();
		}

		if (Input.GetMouseDiffY() != 0) {
			m_pitch -= Input.GetMouseDiffY()*speed_camera*getDeltaTime();
			/*if (m_pitch > max_pitch)
				m_pitch = max_pitch;
			if (m_pitch < min_pitch)
				m_pitch = min_pitch;*/
		}
	
		if (Input.GetRightStickX() != 0.0f) {
			m_yaw = -Input.GetRightStickX()*speed_camera*5;
		}
		if (Input.GetRightStickY() != 0.0f) {
			m_pitch = -Input.GetRightStickY()*speed_camera*5;
			/*if (m_pitch >= max_pitch)
				m_pitch = max_pitch;
			if (m_pitch < min_pitch)
				m_pitch = min_pitch;*/
		}

		//TODO: mouse wheel, distance
	}

	void update(float dt) {
		updateInput();

		CEntity* e_target = target;
		if (!e_target)
			return;
		TCompTransform* target_tmx = e_target->get<TCompTransform>();
		assert(target_tmx);
		auto target_loc = target_tmx->getPosition();
		target_tmx->getAngles(&yaw, &pitch);
		VEC3 delta = getVectorFromYawPitch(yaw, pitch);
		auto origin = target_loc - delta * distance_to_target;
		origin = origin - target_loc;		//normalize vector, needed for traslation pos
		CEntity* e_owner = CHandle(this).getOwner();
		TCompTransform* my_tmx = e_owner->get<TCompTransform>();
		VEC3 posF = rotateAround(origin, 0.0f ,m_pitch, m_yaw);
		posF = posF + target_loc;			//normalize vector, needed for traslation pos
		my_tmx->lookAt(posF, target_loc);
		
	}
};

#endif
