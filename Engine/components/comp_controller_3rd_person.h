#ifndef INC_COMPONENT_CONTROLLER_3RD_PERSON_H_
#define INC_COMPONENT_CONTROLLER_3RD_PERSON_H_

#include "comp_base.h"
#include "handle/handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_life.h"
#include "comp_msgs.h"
#include "geometry/angular.h"
#include "windows\app.h"
#include "input\input.h"
#include "app_modules/io/io.h"

class TCompController3rdPerson : public TCompBase {
	float		yaw;
	float		pitch;
	float		distance_to_target;
	VEC3		position_diff;
	float		speed_camera;
	float		m_yaw;
	float		m_pitch;
	float		min_pitch = -1.0f;
	float		max_pitch = 0.25f;
	float		rotation_sensibility;
	bool		y_axis_inverted;
	bool		x_axis_inverted;

public:
	CHandle		target;

	TCompController3rdPerson()
		: yaw(deg2rad(0.f))
		, pitch(deg2rad(0.f))
		, distance_to_target(5.0f)
		, position_diff(VEC3(0, 0, 0))
		, speed_camera(2.0f)
		, m_yaw(0.0f)
		, m_pitch(0.0f)
		, rotation_sensibility(deg2rad(45.0f)/250.0f)
	{}

	void onCreate(const TMsgEntityCreated& msg) {
		CApp& app = CApp::get();
		CEntity* e_owner = CHandle(this).getOwner();

		//init aspect/ratio from screen
		TCompCamera *camera = e_owner->get<TCompCamera>();
		float ar = (float)app.getXRes() / (float)app.getYRes();
		camera->setAspectRatio(ar);

		//read y-axis inverted
		y_axis_inverted = GetPrivateProfileIntA("controls",
			"y-axis_inverted",
			1,
			app.file_options.c_str());

		//read x-axis inverted
		x_axis_inverted = GetPrivateProfileIntA("controls",
			"x-axis_inverted",
			1,
			app.file_options.c_str());
	}

	void onSetTarget(const TMsgSetTarget& msg) {
		target = msg.target;
	}

	void updateInput() {
		if(x_axis_inverted)	yaw -= io->mouse.dx * rotation_sensibility*speed_camera;
		else yaw += io->mouse.dx * rotation_sensibility*speed_camera;
		if(y_axis_inverted) pitch -= io->mouse.dy * rotation_sensibility*speed_camera;
		else pitch += io->mouse.dy * rotation_sensibility*speed_camera;

		if (pitch >= max_pitch) {
			pitch = max_pitch;
		}

		if (pitch <= min_pitch) {
			pitch = min_pitch;
		}

		//TODO: mouse wheel, distance
		/**** WHEEL EXAMPLE
		if (io->mouse.wheel) {
			pitch += io->mouse.wheel * 8;
		}
		*/
	}

	void update(float dt) {
		CEntity* e_target = target;
		if (!e_target)
			return;

		updateInput();

		TCompTransform* target_tmx = e_target->get<TCompTransform>();
		assert(target_tmx);
		auto target_loc = target_tmx->getPosition();
		VEC3 delta = getVectorFromYawPitch(yaw, pitch);
		auto origin = target_loc - delta * distance_to_target;

		CEntity* e_owner = CHandle(this).getOwner();
		TCompTransform* my_tmx = e_owner->get<TCompTransform>();
		my_tmx->lookAt(origin, target_loc);
	}

	void renderInMenu() {
		ImGui::SliderFloat("rot_speed", &speed_camera, -2.0f, 2.0f);
	}
};

#endif
