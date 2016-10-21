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
#include "app_modules/io/input_wrapper.h"
#include "comp_charactercontroller.h"
#include "comp_camera_main.h"

#define THIRD_PERSON_CONTROLLER_PLAYER_DIST					2.0f
#define THIRD_PERSON_CONTROLLER_MOLE_DIST					5.f
#define THIRD_PERSON_CONTROLLER_SCIENTIST_DIST				4.f
#define THIRD_PERSON_CONTROLLER_PLAYER_POS_OFFSET_Y			-1.0f
#define THIRD_PERSON_CONTROLLER_MOLE_POS_OFFSET_Y			0.f
#define THIRD_PERSON_CONTROLLER_SCIENTIST_POS_OFFSET_Y		0.f
#define E_NUMBER											2.71828f
#define PI_NUMBER											3.14159f
#define	VIBRATION_SPEED										9.f
#define VIBRATION_STOP_SPEED								7.5f

class TCompController3rdPerson : public TCompBase {
	float		yaw;
	float		pitch;
	float		distance_to_target;
	float		speed_camera;
	float		speed_camera_unlocked;
	float		m_yaw;
	float		m_pitch;
	float		min_pitch = -1.0f;
	float		max_pitch = 0.25f;
	float		rotation_sensibility;
	bool		y_axis_inverted;
	bool		x_axis_inverted;
	bool		input_enabled = true;
	bool		pitch_disabled = false;
	bool		orbit_mode = false;
	VEC3		position_diff;
	VEC3		offset;
	// camera vibration
	bool		vibrating = false;
	bool		stopping_vibration = false;
	float		vibration_x_max;
	float		vibration_y_max;
	float		vibration_t = 0.f;

public:
	CHandle		target;

	TCompController3rdPerson()
		: yaw(deg2rad(0.f))
		, pitch(deg2rad(0.f))
		, distance_to_target(5.0f)
		, position_diff(VEC3(0, 0, 0))
		, speed_camera(2.0f)
		, speed_camera_unlocked(5.0f)
		, m_yaw(0.0f)
		, m_pitch(0.0f)
		, rotation_sensibility(deg2rad(45.0f) / 250.0f)
	{}

	float GetPositionDistance() const {
		return distance_to_target;
	}

	void setDistanceToTarget(float distance) {
		distance_to_target = distance;
	}

	void setSpeed(float speed) {
		speed_camera = speed;
	}

	void setSpeedUnlocked(float speed) {
		speed_camera_unlocked = speed;
	}

	void setRotationSensibility(float sensibility) {
		rotation_sensibility = deg2rad(sensibility) / 250.0f;
	}

	void setPositionOffset(VEC3 offset) {
		position_diff = offset;
	}

	// camera vibration
	void startVibration(float x_max, float y_max) {

		// set the new parameters
		vibrating = true;
		vibration_t = 0.f;
		vibration_x_max = x_max;
		vibration_y_max = y_max;
	}

	void stopVibration() {
		vibration_t = 0.f;
		stopping_vibration = true;
	}

	void onCreate(const TMsgEntityCreated& msg) {
		CApp& app = CApp::get();

		//init aspect/ratio from screen
		GET_MY(camera, TCompCameraMain);
		float ar = (float)app.getXRes() / (float)app.getYRes();
		camera->setAspectRatio(ar);

		std::map<std::string, float> options = readIniAtrData(app.file_options_json, "controls");
		//read y-axis inverted, "0 != " to convert uint to bool more efficient
		y_axis_inverted = 0 == (int)options["y-axis_inverted"];
		//read x-axis inverted, "0 != " to convert uint to bool more efficient
		x_axis_inverted = 0 == (int)options["x-axis_inverted"];
	}

	void onSetTarget(const TMsgSetTarget& msg) {
		switch (msg.who) {
		case PLAYER:
			distance_to_target = THIRD_PERSON_CONTROLLER_PLAYER_DIST;
			position_diff = VEC3(0.f, THIRD_PERSON_CONTROLLER_PLAYER_POS_OFFSET_Y, 0.f);
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
		target = msg.target;
		if (target.isValid()) {
			GET_COMP(tmx_target, target, TCompTransform);
			if (tmx_target) yaw = tmx_target->getYaw();
		}
	}

	void updateInput() {
		float deltaYaw = 0.f;
		float deltaPitch = 0.f;

		if (x_axis_inverted)	deltaYaw -= controller->RXNormalized() * rotation_sensibility * speed_camera;
		else					deltaYaw += controller->RXNormalized() * rotation_sensibility * speed_camera;

		if (pitch_disabled) {
			deltaPitch += rotation_sensibility * speed_camera *2.f;
		}
		else {
			if (y_axis_inverted)	deltaPitch -= controller->RYNormalized() * rotation_sensibility * speed_camera;
			else					deltaPitch += controller->RYNormalized() * rotation_sensibility * speed_camera;
		}
		yaw = MOD_YAW(yaw + deltaYaw);

		pitch += deltaPitch;

		if (pitch >= max_pitch) {
			pitch = max_pitch;
		}
		else if (pitch <= min_pitch) {
			pitch = min_pitch;
		}
	}
	void updateOrbit() {
		float deltaYaw = rotation_sensibility * speed_camera;
		yaw = MOD_YAW(yaw + deltaYaw);
	}

	void update(float dt) {
		if (!GameController->GetFreeCamera())
			personThirdController(dt);

		else
			unlockedCameraController();
	}

	//default behaviour
	void personThirdController(float dt) {
		if (!target.isValid()) return;

		if (orbit_mode) updateOrbit();
		else if (input_enabled) updateInput();

		GET_COMP(target_tmx, target, TCompTransform);
		assert(target_tmx);

		auto target_loc = target_tmx->getPosition();
		VEC3 delta = getVectorFromYawPitch(yaw, pitch);
		auto origin = target_loc - delta * distance_to_target;

		GET_MY(my_tmx, TCompTransform);
		if (!my_tmx) return;

		GET_MY(obtarged, TCompController3rdPerson);
		if (!obtarged) return;
		CHandle targetowner = obtarged->target;

		GET_COMP(targettrans, targetowner, TCompTransform);
		if (!targettrans) return;

		GET_COMP(cc, targetowner, TCompCharacterController);
		if (!cc) return;

		offset = position_diff - VEC3(targettrans->getLeft()*cc->GetRadius());
		my_tmx->lookAt(origin, target_loc);
		//Aplicar offset y vibracion, si es necesario

		float x_pos = 0.f;
		float y_pos = 0.f;

		if (vibrating) {
			// starting to vibrate
			if (!stopping_vibration) {
				vibration_t += VIBRATION_SPEED / 100.f;
				// x vibration
				x_pos = cos(2.f*PI_NUMBER*vibration_t)*pow(E_NUMBER, vibration_t);
				if (x_pos > vibration_x_max)
					x_pos = vibration_x_max;
				else if (x_pos < -vibration_x_max)
					x_pos = -vibration_x_max;
				// y vibration
				y_pos = cos(2.f*PI_NUMBER*vibration_t)*pow(E_NUMBER, vibration_t);
				if (y_pos > vibration_y_max)
					y_pos = vibration_y_max;
				else if (y_pos < 0.f)
					y_pos = 0.f;
				else if (y_pos < -vibration_y_max)
					y_pos = -vibration_y_max;
			}
			// stoping vibration
			else {
				vibration_t += VIBRATION_STOP_SPEED / 100.f;
				// x vibration
				x_pos = cos(2.f*PI_NUMBER*vibration_t)*pow(E_NUMBER, -vibration_t);
				if (x_pos > vibration_x_max)
					x_pos = vibration_x_max;
				else if (x_pos < -vibration_x_max)
					x_pos = -vibration_x_max;
				// y vibration
				y_pos = cos(2.f*PI_NUMBER*vibration_t)*pow(E_NUMBER, -vibration_t);
				if (y_pos > vibration_y_max)
					y_pos = vibration_y_max;
				else if (y_pos < 0.f)
					y_pos = 0.f;
				else if (y_pos < -vibration_y_max)
					y_pos = -vibration_y_max;

				if (vibration_t >= 10.f) {
					vibrating = false;
					stopping_vibration = false;
				}
			}
		}

		VEC3 vibration = VEC3(x_pos, y_pos, 0.f);
		my_tmx->setPosition(my_tmx->getPosition() + vibration + position_diff + VEC3_UP * 2.f);
	}

	VEC3 GetOffset() const { return offset; }

	void unlockedCameraController() {
#ifndef FINAL_BUILD
		GET_MY(my_tmx, TCompTransform);
		VEC3 origin = my_tmx->getPosition();
		float dt = getDeltaTime(true);
		if (!ImGui::GetIO().WantTextInput)
		{
			if (controller->IsMoveForward())
				origin += my_tmx->getFront() * dt * speed_camera_unlocked;
			if (controller->IsMoveBackWard())
				origin -= my_tmx->getFront() * dt * speed_camera_unlocked;
			if (controller->IsMoveLeft())
				origin += my_tmx->getLeft() * dt * speed_camera_unlocked;
			if (controller->IsMoveRight())
				origin -= my_tmx->getLeft() * dt * speed_camera_unlocked;
			if (controller->IsCameraUpPressing())
				origin.y += dt * speed_camera_unlocked;
			if (controller->IsCameraDownPressing())
				origin.y -= dt * speed_camera_unlocked;
			if (controller->isTeleportComboButtonPressed()) {
				CHandle player = tags_manager.getFirstHavingTag("player");
				if (player.isValid()) {
					GET_COMP(cc, player, TCompCharacterController);
					if (cc) cc->teleport(my_tmx->getPosition() + my_tmx->getFront() * 2);
				}
				origin.y -= dt * speed_camera_unlocked;
			}
			/*
			if (io->mouse.wheel != 0) {
				speed_camera_unlocked += io->mouse.wheel * 20 * dt;
				if (speed_camera_unlocked < 0)
					speed_camera_unlocked = 0.0f;
			}
			*/
		}
		yaw -= controller->MouseDeltaX() * rotation_sensibility;
		pitch -= controller->MouseDeltaY() * rotation_sensibility;

		VEC3 front = getVectorFromYawPitch(yaw, pitch);

		my_tmx->lookAt(origin, origin + front);
#endif
	}

	void onSetControllable(const TMsgSetControllable& msg)
	{
		input_enabled = msg.control;
	}

	void StartOrbit() { orbit_mode = true; }
	void StopOrbit() { orbit_mode = false; }
	void SetPitchEnabled(bool pitch_enabled) { pitch_disabled = !pitch_enabled; }

	void renderInMenu() {
		ImGui::DragFloat("rot_speed", &speed_camera, -0.1f, 0.1f);
		ImGui::DragFloat("distanceToTarget", &distance_to_target, 0.0001f, 0.1f);
		ImGui::DragFloat3("positionDiff", &position_diff.x, -0.1f, 0.1f);
		ImGui::Checkbox("Input enabled", &input_enabled);
		ImGui::Checkbox("Pitch Disabled", &pitch_disabled);
	}
};

#endif
