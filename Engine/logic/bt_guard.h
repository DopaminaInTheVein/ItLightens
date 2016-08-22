#ifndef INC_BT_GUARD_H_
#define INC_BT_GUARD_H_

#include "npc.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "skeleton_controllers/skc_guard.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include <vector>

#define DIST_SQ_SOUND_DETECTION			35
#define PLAYER_CENTER_Y					0.5f

#define DIST_SQ_REACH_PNT_INI			10
#define DIST_SQ_SHOT_AREA_ENTER_INI		50
#define DIST_SQ_SHOT_AREA_LEAVE_INI		100
#define DIST_RAYSHOT_INI				20
#define DIST_SQ_PLAYER_DETECTION_INI	150
#define DIST_SQ_PLAYER_LOST_INI			200
#define SPEED_WALK_INI					3
#define CONE_VISION_INI					deg2rad(60)
#define SPEED_ROT_INI					deg2rad(100)
#define DAMAGE_LASER_INI				30

#define ST_SELECT_ACTION	"select_action"
#define ST_NEXT_ACTION		"next_action"
#define ST_SEEK_POINT		"seek_point"
#define ST_WAIT_NEXT		"wait_next"
#define ST_LOOK_POINT		"look_point"
#define ST_CHASE			"chase"
#define ST_SHOOT			"shoot"
#define ST_SOUND_DETECTED	"sound_detected"
#define ST_LOOK_ARROUND		"look_arround"
#define ST_SHOOTING_WALL	"shootingWall"

class guard_alert {
public:
	VEC3 guard_position;
	VEC3 alert_position;
	float timer;
};

class bt_guard : public npc, public TCompBase
{
	//Main attritbutes
	float DIST_REACH_PNT;
	float PLAYER_DETECTION_RADIUS;
	float DIST_SQ_SHOT_AREA_ENTER;
	float DIST_SQ_SHOT_AREA_LEAVE;
	float DIST_RAYSHOT;
	float DIST_SQ_PLAYER_DETECTION;
	float DIST_SQ_PLAYER_LOST;
	float SPEED_WALK;
	float SHOOT_PREP_TIME;
	float MIN_SQ_DIST_TO_PLAYER;
	float CONE_VISION;
	float SPEED_ROT;
	float DAMAGE_LASER;
	float MAX_REACTION_TIME;
	float MAX_BOX_REMOVAL_TIME;
	float BOX_REMOVAL_ANIM_TIME;
	float MAX_SEARCH_DISTANCE;
	float LOOK_AROUND_TIME;
	float GUARD_ALERT_TIME;
	float GUARD_ALERT_RADIUS;
	float RANDOM_POINT_MAX_DISTANCE;
	VEC4 SHOT_OFFSET;
	//from bombs
	float reduce_factor;
	float t_reduceStats_max;
	float t_reduceStats;
	float MAX_STUCK_TIME;
	float UNSTUCK_DISTANCE;

	//Handles & More
	CHandle myHandle;
	CHandle myParent;
	CHandle thePlayer;
	TCompTransform * getTransform();
	TCompCharacterController* getCC();
	CEntity* getPlayer();

	//Cambio Malla
	//TCompRenderStaticMesh* mesh;
	//string pose_idle_route;
	//string pose_shoot_route;
	//string pose_run_route;
	//string last_pose = "";

	//Cambio anim
	SkelControllerGuard * animController;

	//Debug
	//____TIMER_DECLARE_(timerDebug, 2.0f);

	//KeyPoints
	enum KptType { Seek = 0, Look };
	struct KeyPoint {
		KptType type;
		VEC3 pos;
		float time;
		KeyPoint() : type(KptType::Seek), pos(VEC3(0, 0, 0)), time(0) {};
		KeyPoint(VEC3 p) : type(KptType::Seek), pos(p), time(0) {};
		KeyPoint(KptType t, VEC3 p) : type(t), pos(p), time(0) {};
		KeyPoint(KptType t, VEC3 p, float temps) : type(t), pos(p), time(temps) {};
	};
	static map<string, KptType > kptTypes;

	bool noShoot;
	bool sendMsgDmg = false;
	std::vector<KeyPoint> keyPoints;
	int curkpt;
	VEC3 player_last_seen_point;
	VEC3 noisePoint;
	VEC3 search_player_point;
	bool patrolling = false;
	bool noiseHeard = false;
	bool playerLost = false;
	bool looking_player = false;
	// stuck management
	float stuck_time = 0.f;
	bool stuck = false;
	bool reoriented = false;
	int direction = 0;
	VEC3 unstuck_target;
	VEC3 last_position;
	VEC3 dest_shoot;
	// reaction time management
	bool player_detected_start = false;
	float reaction_time = -1.0f;
	// box removal management
	CHandle box_to_remove;
	bool remove_box_ready = true;
	float remove_box_time = -1.0f;
	float removing_box_animation_time = 0.f;
	float looking_around_time = 0.f;
	// shoot parameters
	float shoot_preparation_time = 0.f;

	//Toggles
	bool formation_toggle;
	VEC3 formation_point;
	VEC3 formation_dir;

	//Correa
	VEC3 jurCenter;
	float jurRadiusSq;

	//Aux actions
	void goTo(const VEC3& dest);
	void goForward(float stepForward);
	bool turnTo(VEC3 dest, bool wide = false);
	bool turnToPlayer();
	void lookAtPlayer();
	void lookAtFront();
	//VEC3 generateRandomPoint(); THIS IS NOT USED!

	//Aux checks
	bool playerVisible(bool check_raycast = true);
	bool playerTooNear();
	bool boxMovingDetected();
	bool inJurisdiction(VEC3);
	bool outJurisdiction(VEC3);
	bool canHear(VEC3, float);
	/*bool isPathObtained = false;
	bool isPathObtainedAccessible = false;*/

	//Times and similars
	float timeWaiting;
	float deltaYawLookingArround;
	void resetTimers();

	//Raycast
	bool rayCastToPlayer(int types, float& distRay, PxRaycastBuffer& hit);
	bool rayCastToFront(int types, float& distRay, PxRaycastBuffer& hit);
	bool rayCastToTransform(int types, float& distRay, PxRaycastBuffer& hit, TCompTransform* transform);
	bool shootToPlayer();
	void drawShot(VEC3 dest);
	void removeBox(CHandle box_handle);

	bool stunned;
	bool shooting = false;
	bool forced_move = false;

	// the nodes
	static map<string, btnode *>tree;
	// the C++ functions that implement node actions, hence, the behaviours
	static map<string, btaction> actions;
	// the C++ functions that implement conditions
	static map<string, btcondition> conditions;
	// the events that will be executed by the decoratos
	static map<string, btevent> events;
	// tree root
	static btnode* root;

public:
	//public for LUA
	bool isInFirstSeekPoint();

	//conditions
	bool guardStuck();
	bool playerStunned();
	bool playerNear();
	bool playerDetected();
	bool playerOutOfReach();
	bool guardAlerted();
	//toggle conditions
	bool checkFormation();
	//actions
	int actionUnstuckTurn();
	int actionUnstuckMove();
	int actionStunned();
	int actionStepBack();
	int actionReact();
	int actionChase();
	int actionPrepareToAbsorb();
	int actionAbsorb();
	int actionShootWall();
	int actionRemoveBox();
	int actionSearch();
	int actionMoveAround();
	int actionLookAround();
	int actionSeekWpt();
	int actionNextWpt();
	int actionWaitWpt();
	//toggle actions
	int actionGoToFormation();
	int actionTurnToFormation();
	int actionWaitInFormation();
	//Toggle enabling/disabling functions
	void toggleFormation() {
		setCurrent(NULL);
		formation_toggle = !formation_toggle;
	}
	VEC3 getFormationPoint() {
		return formation_point;
	}
	//functions that allow access to the static maps
	map<string, btnode *>* getTree() override {
		return &tree;
	}

	map<string, btaction>* getActions() override {
		return &actions;
	}

	map<string, btcondition>* getConditions() override {
		return &conditions;
	}

	map<string, btevent>* getEvents() override {
		return &events;
	}

	btnode** getRoot() override {
		return &root;
	}

	void Init();
	void noise(const TMsgNoise& msg);
	void readIniFileAttr();
	void goToPoint(VEC3 dest);
	void checkStopDamage();
	bool isPatrolling();
	//From bombs
	void reduceStats();
	void resetStats();
	void onMagneticBomb(const TMsgMagneticBomb& msg);
	void onStaticBomb(const TMsgStaticBomb& msg);
	void onOverCharged(const TMsgOverCharge& msg);
	void onBoxHit(const TMsgBoxHit& msg);

	//TODO: remove, testing gameplay
	void artificialInterrupt();

	bool getUpdateInfo() {
		animController = GETH_MY(SkelControllerGuard);
		//dbg("Time looking for: %f\n", looking_around_time);
		return true;
	}

	void update(float dt) {
		if (!isInRoom(myParent))return;
		TCompTransform * t = compBaseEntity->get<TCompTransform>();
		Debug->DrawLine(t->getPosition(), player_last_seen_point, VEC3(0, 1, 0));
		Debug->DrawLine(t->getPosition(), t->getPosition() + t->getFront(), VEC3(1, 1, 0));
		Debug->DrawLine(t->getPosition(), t->getPosition() + t->getLeft(), VEC3(1, 1, 0));
		if (curkpt >= 0) Debug->DrawLine(getTransform()->getPosition(), keyPoints[curkpt].pos + VEC3_UP, VEC3(1.f, 1.f, 1.f));
		if (t_reduceStats > 0.0f) {	//CRISTIAN!!! ordenalo como prefieras
			t_reduceStats -= getDeltaTime();
			if (t_reduceStats <= 0.0f) {
				t_reduceStats = 0.0f;
				resetStats();
			}
		}
		// stuck management
		float distance = simpleDistXZ(last_position, t->getPosition());
		if (distance <= 0.75f*getDeltaTime()*SPEED_WALK) {
			stuck_time += getDeltaTime();
			if (stuck_time > MAX_STUCK_TIME && !stuck) {
				stuck = true;
				setCurrent(NULL);
			}
		}
		else {
			stuck_time = 0.f;
		}

		last_position = t->getPosition();

		if (!forced_move) Recalc();

		updateLookAt();
		//animController.update();
	}

	void updateLookAt();

	void render();
	void renderInMenu();
	bool load(MKeyValue& atts);
	std::string getKpTypeStr(bt_guard::KptType type);
	bool save(std::ofstream& os, MKeyValue& atts);

	//Cambio Malla
	//void ChangePose(string new_pose_route);

	float timerStunt, _timerStunt;
	____TIMER_DECLARE_VALUE_(timerShootingWall, 8)
};

#endif