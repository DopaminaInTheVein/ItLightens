#ifndef INC_BT_GUARD_H_
#define INC_BT_GUARD_H_

#include "bt.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include <vector>

#define DIST_SQ_SOUND_DETECTION			400
#define PLAYER_CENTER_Y					0.5f

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

//Cambio malla
struct TCompRenderStaticMesh;
class TCompCharacterController;

class bt_guard : public TCompBase, public bt
{
	//Main attritbutes
	float DIST_SQ_REACH_PNT;
	float DIST_SQ_SHOT_AREA_ENTER;
	float DIST_SQ_SHOT_AREA_LEAVE;
	float DIST_RAYSHOT;
	float DIST_SQ_PLAYER_DETECTION;
	float DIST_SQ_PLAYER_LOST;
	float SPEED_WALK;
	float CONE_VISION;
	float SPEED_ROT;
	float DAMAGE_LASER;
	//from bombs
	float reduce_factor;
	float t_reduceStats_max;
	float t_reduceStats;

	//Handles & More
	CHandle myHandle;
	CHandle myParent;
	CHandle thePlayer;
	TCompTransform * getTransform();
	TCompCharacterController* getCC();
	CEntity* getPlayer();

	//Cambio Malla
	TCompRenderStaticMesh* mesh;
	string pose_idle_route;
	string pose_shoot_route;
	string pose_run_route;
	string last_pose = "";

	//Debug
	//____TIMER_DECLARE_(timerDebug, 2.0f);

	//KeyPoints
	enum KptType { Seek, Look };
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
	VEC3 noisePoint;
	bool noiseHeard = false;
	bool playerLost = false;

	//Correa
	VEC3 jurCenter;
	float jurRadiusSq;

	//Aux actions
	void goTo(const VEC3& dest);
	void goForward(float stepForward);
	bool turnTo(VEC3 dest);

	//Aux checks
	bool playerVisible();
	bool inJurisdiction(VEC3);
	bool outJurisdiction(VEC3);
	bool canHear(VEC3, float);

	//Times and similars
	float timeWaiting;
	float deltaYawLookingArround;
	void resetTimers();

	//Raycast
	bool rayCastToPlayer(int types, float& distRay, PxRaycastBuffer& hit);
	void shootToPlayer();

	bool stunned;

	// the nodes
	static map<string, btnode *>tree;
	// the C++ functions that implement node actions, hence, the behaviours
	static map<string, btaction> actions;
	// the C++ functions that implement conditions
	static map<string, btcondition> conditions;
	// tree root
	static btnode* root;

public:
	//conditions
	bool playerStunned();
	bool playerDetected();
	bool playerOutOfReach();
	bool guardAlerted();
	//actions
	int actionStunned();
	int actionChase();
	int actionAbsorb();
	int actionShootWall();
	int actionSearch();
	int actionLookAround();
	int actionSeekWpt();
	int actionNextWpt();
	int actionWaitWpt();

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

	btnode** getRoot() override {
		return &root;
	}

	void Init();
	void noise(const TMsgNoise& msg);
	void readIniFileAttr();

	//From bombs
	void reduceStats();
	void resetStats();
	void onMagneticBomb(const TMsgMagneticBomb& msg);
	void onStaticBomb(const TMsgStaticBomb& msg);

	//TODO: remove, testing gameplay
	void artificialInterrupt();

	void update(float dt) {
		if (t_reduceStats > 0.0f) {	//CRISTIAN!!! ordenalo como prefieras
			t_reduceStats -= getDeltaTime();
			if (t_reduceStats <= 0.0f) {
				t_reduceStats = 0.0f;
				resetStats();
			}
		}
		Recalc();
	}

	void render();
	void renderInMenu();
	bool load(MKeyValue& atts);

	//Cambio Malla
	void ChangePose(string new_pose_route);

	float timerStunt, _timerStunt;
	____TIMER_DECLARE_VALUE_(timerShootingWall, 8)
};

#endif