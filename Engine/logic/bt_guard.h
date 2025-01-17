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
	//static float SHOT_OFFSET;
	float PLAYER_DETECTION_RADIUS;
	float DIST_SQ_SHOT_AREA_ENTER;
	float DIST_SQ_SHOT_AREA_LEAVE;
	float DIST_RAYSHOT;
	float DIST_SQ_PLAYER_DETECTION;
	float DIST_SQ_PLAYER_LOST;
	float SHOOT_PREP_TIME;
	float MIN_SQ_DIST_TO_PLAYER;
	float CONE_VISION;
	float DAMAGE_LASER;
	float MAX_REACTION_TIME;
	float MAX_SEARCH_DISTANCE;
	float LOOK_AROUND_TIME;
	float GUARD_ALERT_TIME;
	float GUARD_ALERT_RADIUS;
	//from bombs
	float reduce_factor;
	float t_reduceStats_max;
	float t_reduceStats;

	//Handles & More
	CHandle myHandle;
	CHandle myParent;
	CHandle thePlayer;
	TCompTransform * my_tmx;
	TCompTransform * player_tmx;
	TCompCharacterController* my_cc;
	TCompCharacterController* player_cc;

	//TCompTransform * getTransform() override;
	//TCompCharacterController* getCC();
	//CEntity* getPlayer();

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
	bool shoot_prepared = false;
	float shoot_preparation_time = 0.f;

	//Toggles
	bool formation_toggle;
	VEC3 formation_point;
	VEC3 formation_dir;

	//Correa
	VEC3 jurCenter;
	float jurRadiusSq;

	//bool turnTo(VEC3 dest, bool wide = false);
	bool turnToPlayer();
	void lookAtPlayer();
	void lookAtFront();

	//Aux checks
	bool playerVisible(bool check_raycast = true);
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
	void shootToPlayer();
	void drawShot(VEC3 dest);
	void removeBox(CHandle box_handle);

	bool stunned = false;;
	bool stunt_recover = true;
	bool shooting = false;
	bool shooting_backwards = false;
	bool forced_move = false;

	// PANIC mode
	bool force_patrol = false;

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

	int step_counter = 0;
	bool chasing = false;
	static int guards_chasing;

public:
	//public for LUA
	bool isInFirstSeekPoint();

	//conditions
	bool playerStunned();
	bool playerNear();
	bool playerDetected();
	bool playerOutOfReach();
	bool guardAlerted();
	//toggle conditions
	bool checkFormation();
	//actions
	int actionStunned();
	int actionStepBack();
	int actionReact();
	int actionChase();
	int actionPrepareToAbsorb();
	int actionAbsorb();
	int actionShootWall();
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
	void onStaticBomb(const TMsgStaticBomb& msg);
	void onOverCharged(const TMsgOverCharge& msg);
	void onBoxHit(const TMsgBoxHit& msg);

	//TODO: remove, testing gameplay
	void artificialInterrupt();

	bool getUpdateInfo() override;
	void update(float dt) {
		//if (DEBUG_CODE) {
		//	for (int i = 1; i < pathWpts.size(); i++) {
		//		if (i > 0) {
		//			Debug->DrawLine(pathWpts[i - 1] + VEC3_UP* .2f, pathWpts[i] + VEC3_UP*.2f, BLUE);
		//		}
		//		i++;
		//	}
		//}
#ifndef NDEBUG
		if (DEBUG_CODE) {
			Debug->DrawLine(my_tmx->getPosition(), player_last_seen_point, VEC3(0, 1, 0));
			Debug->DrawLine(my_tmx->getPosition(), my_tmx->getPosition() + my_tmx->getFront(), VEC3(1, 1, 0));
			Debug->DrawLine(my_tmx->getPosition(), my_tmx->getPosition() + my_tmx->getLeft(), VEC3(1, 1, 0));
		}
		if (curkpt >= 0) Debug->DrawLine(getTransform()->getPosition(), keyPoints[curkpt].pos + VEC3_UP, VEC3(1.f, 1.f, 1.f));
#endif
		if (t_reduceStats > 0.0f) {	//CRISTIAN!!! ordenalo como prefieras
			t_reduceStats -= getDeltaTime();
			if (t_reduceStats <= 0.0f) {
				t_reduceStats = 0.0f;
				resetStats();
			}
		}
		// stuck management
		updateStuck();

		if (!forced_move) Recalc();

		updateLookAt();
		//animController.update();
		updateTalk("Guard", CHandle(this).getOwner());

		// PANIC MODE
		if (io->keys['M'].becomesPressed()) {
			setCurrent(NULL);
			decreaseChaseCounter();
			force_patrol = !force_patrol;
			//If was shooting...
			if (shooting) {
				TMsgDamageSpecific dmg;
				dmg.source = MY_NAME;
				dmg.type = Damage::ABSORB;
				dmg.actived = false;
				thePlayer.sendMsg(dmg);
			}
		}
	}

	void updateLookAt();

	void render();
	void renderInMenu();
	bool load(MKeyValue& atts);
	std::string getKpTypeStr(bt_guard::KptType type);
	bool save(std::ofstream& os, MKeyValue& atts);

	void changeCommonState(std::string);
	void onGetWhoAmI(TMsgGetWhoAmI& msg);
	int getStepCounter() { return step_counter; }
	void increaseChaseCounter();
	void decreaseChaseCounter();

	float timerStunt, _timerStunt;

	void onDifficultyChanged(const TMsgDifficultyChanged&);

	TCompTransform * getTransform() { return my_tmx; };
	CHandle getParent() override { return MY_OWNER; };
	TCompCharacterController * getCC() {
		if (!my_cc) my_cc = GETH_MY(TCompCharacterController);
		return my_cc;
	}

	____TIMER_DECLARE_VALUE_(timerShootingWall, 8)
};

#endif