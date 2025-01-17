#ifndef INC_PC_BASE_H_
#define	INC_PC_BASE_H_

#include "logic/aicontroller.h"
#include "logic/sbb.h"

#include "components/comp_base.h"
#include "handle/handle.h"
#include "components/comp_msgs.h"

#include "camera/camera.h"

class CEntity;

template< class TObj >
class CObjectManager;
class TCompCharacterController;
class TCompSenseVision;

class CPlayerBase : public aicontroller, public TCompBase {
protected:

	virtual map<string, statehandler>* getStatemap();

	//Enabled
	bool controlEnabled = false;
	bool inputEnabled = true;
	bool only_sense = false;
	bool dead = false;

	// CountDown To Unpossess
	static float possessionCooldown;

	// Map for debug on ImGui
	static std::map<int, std::string> out;

	// Controller input class
	CHandle camera;

	float dt;

	// Main attributes
	float player_max_speed;
	float player_rotation_speed;
	float energy_default_decrease = 0.0f;
	float energy_sense_decrease = 0.0f;
	float energy_decrease = 0.0f;
	float energy_damage;
	float jimpulse;
	float left_stick_sensibility;
	float camera_max_height;
	float camera_min_height;
	bool onGround = true;
	bool moving = false;
	float player_curr_speed = 0.0f;
	float jspeed = 0.0f;
	float time_start_falling = 0.f;
	float max_time_start_falling = 0.2f; //TODO JSON

	//CObjectManager<CPlayerBase> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CEntity *myEntity = nullptr;
	TCompTransform *transform;
	CHandle h_sense_vision;
	TCompSenseVision* sense_vision;

	VEC3 directionLateral = VEC3(0, 0, 0);
	VEC3 directionVertical = VEC3(0, 0, 0);
	VEC3 directionForward = VEC3(0, 0, 1);
	VEC3 directionJump = VEC3(0, 0, 0);
	float rotate = 0;
	float rotateXY = 0;
	float starting_player_y = 0;
	float player_y = 0;
	bool gravity_active = true;

	TCompCharacterController *cc = nullptr;

	// Cinematic Target
	VEC3 cinematicTargetPos;
	float cinematicTargetYaw;
	bool onCinematic = false;
	std::string cinematicEndCode;
	float epsilonPos = 0.2f;
	float epsilonYaw = deg2rad(1);

	//virtual needed for poses right now
	virtual void UpdateInputActions();
	virtual void UpdateMoves();
	virtual bool UpdateMovDirection();
	virtual void UpdateMovingWithOther();
	virtual void UpdateJumpState();
	virtual void UpdateDirection();
	//virtual void UpdateAnimation() {}
	virtual void UpdateCinematic(float elapsed);
	virtual void UpdateSenseVision();
	virtual void ChangeCommonState(std::string) {}
	virtual bool canJump();

	void energyDecreasal(float howmuch);
	bool checkDead();
	void orbitCameraDeath();
	bool getUpdateInfo() override;
	bool checkFalling();
	float getLife();
	void setLife(float new_life);

	bool turnTo(TCompTransform*);
	bool turnTo(VEC3 target);

	void setControllable(bool);

public:
	static CHandle handle_player;
	CPlayerBase();
	void init() {
		initBaseAttributes();
		Init();
	}
	bool getEnabled() { return controlEnabled; }
	void update(float elapsed); //deberia ser const pero Recalc no lo es  e historias
	void addBasicStates();
	virtual void initBaseAttributes();
	void onSetCamera(const TMsgSetCamera& msg);
	void onSetControllable(const TMsgSetControllable& msg);
	void onSetOnlySense(const TMsgSetOnlySense& msg);
	void onGoAndLook(const TMsgGoAndLook& msg);
	void stopMovement();
	virtual void myUpdate(); // deberia ser abstracta pero peta

	// Player states
	virtual void Idle();
	virtual void Jump();
	void Die();
	void Win();

	void Jumping();
	virtual void Falling();
	virtual void StartFalling();
	virtual void updateFalling();
	virtual void Moving();

	virtual void update_msgs() {}
	void renderInMenu();

	bool isMoving() { return moving; }
};

#endif