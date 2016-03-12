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

#define THIRD_PERSON_CONTROLLER_PLAYER_DIST				1.6f
#define THIRD_PERSON_CONTROLLER_SPEEDY_DIST				5.f
#define THIRD_PERSON_CONTROLLER_MOLE_DIST				5.f
#define THIRD_PERSON_CONTROLLER_SCIENTIST_DIST			5.f
#define THIRD_PERSON_CONTROLLER_PLAYER_POS_OFFSET_Y			-1.1f
#define THIRD_PERSON_CONTROLLER_SPEEDY_POS_OFFSET_Y			0.f
#define THIRD_PERSON_CONTROLLER_MOLE_POS_OFFSET_Y			0.f
#define THIRD_PERSON_CONTROLLER_SCIENTIST_POS_OFFSET_Y		0.f

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
		, rotation_sensibility(deg2rad(45.0f) / 250.0f)
	{}

	void onCreate(const TMsgEntityCreated& msg) {
		CApp& app = CApp::get();
		CEntity* e_owner = CHandle(this).getOwner();

		//init aspect/ratio from screen
		TCompCamera *camera = e_owner->get<TCompCamera>();
		float ar = (float)app.getXRes() / (float)app.getYRes();
		camera->setAspectRatio(ar);

		//read y-axis inverted, "0 != " to convert uint to bool more efficient
		y_axis_inverted = 0 != GetPrivateProfileIntA("controls",
			"y-axis_inverted",
			1,
			app.file_options.c_str());

		//read x-axis inverted, "0 != " to convert uint to bool more efficient
		x_axis_inverted = 0 != GetPrivateProfileIntA("controls",
			"x-axis_inverted",
			1,
			app.file_options.c_str());
	}

	void onSetTarget(const TMsgSetTarget& msg) {
		target = msg.target;
		switch (msg.who) {
		case PLAYER:
			distance_to_target = THIRD_PERSON_CONTROLLER_PLAYER_DIST;
			position_diff = VEC3(0.f, THIRD_PERSON_CONTROLLER_PLAYER_POS_OFFSET_Y, 0.f);
			break;
		case SPEEDY:
			distance_to_target = THIRD_PERSON_CONTROLLER_SPEEDY_DIST;
			position_diff = VEC3(0.f, THIRD_PERSON_CONTROLLER_SPEEDY_POS_OFFSET_Y, 0.f);
			break;
		case MOLE:
			distance_to_target = THIRD_PERSON_CONTROLLER_MOLE_DIST;
			position_diff = VEC3(0.f, THIRD_PERSON_CONTROLLER_MOLE_POS_OFFSET_Y, 0.f);
			break;
		case SCIENTIST:
			distance_to_target = THIRD_PERSON_CONTROLLER_SCIENTIST_DIST;
			position_diff = VEC3(0.f, THIRD_PERSON_CONTROLLER_SCIENTIST_POS_OFFSET_Y, 0.f);
			break;
		}
	}

	void updateInput() {
		int movement_x = 0;

		if (io->joystick.drx != 0)
			movement_x = io->joystick.drx;
		else if (io->joystick.rx == io->joystick.min_stick_value)
			movement_x = -2;
		else if (io->joystick.rx == io->joystick.max_stick_value)
			movement_x = 2;

		if (x_axis_inverted)	yaw -= (io->mouse.dx + movement_x)* rotation_sensibility*speed_camera;
		else yaw += (io->mouse.dx + movement_x) * rotation_sensibility*speed_camera;
		if (y_axis_inverted) pitch -= (io->mouse.dy + io->joystick.dry) * rotation_sensibility*speed_camera;
		else pitch += (io->mouse.dy + io->joystick.dry) * rotation_sensibility*speed_camera;

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

		TCompController3rdPerson * obtarged = e_owner->get<TCompController3rdPerson>();
		CHandle targetowner = obtarged->target;
		CEntity* targeted = targetowner;
		TCompLife * targetlife = targeted->get<TCompLife>();
		TCompTransform * targettrans = targeted->get<TCompTransform>();
		CEntity * victoryPoint = tags_manager.getFirstHavingTag(getID("victory_point"));
		TCompTransform * victoryPoint_transform = victoryPoint->get<TCompTransform>();

		if (targetlife->currentlife > 0.0f && 0.5f <= simpleDist(victoryPoint_transform->getPosition(), targettrans->getPosition())) {
			my_tmx->lookAt(origin, target_loc);
			//Aplicar offset
			my_tmx->setPosition(my_tmx->getPosition() + position_diff);
		}
	}

	void renderInMenu() {
		ImGui::SliderFloat("rot_speed", &speed_camera, -2.0f, 2.0f);
		ImGui::SliderFloat("distanceToTarget", &distance_to_target, 0.5f, 10.f);
		ImGui::SliderFloat3("positionDiff", &position_diff.x, -5.f, 5.f);
	}
};

#endif
