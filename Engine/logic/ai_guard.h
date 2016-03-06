#ifndef INC_AIC_GUARD_H_
#define INC_AIC_GUARD_H_

#include "aicontroller.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include <vector>

#define DIST_SQ_REACH_PNT_INI			10
#define DIST_SQ_SHOT_AREA_ENTER_INI		50
#define DIST_SQ_SHOT_AREA_LEAVE_INI		100
#define DIST_RAYSHOT_INI				20
#define DIST_SQ_PLAYER_DETECTION_INI	150
#define DIST_SQ_PLAYER_LOST_INI			200
#define DIST_SQ_SOUND_DETECTION			400
#define SPEED_WALK_INI					3
#define CONE_VISION_INI					deg2rad(60)
#define SPEED_ROT_INI					deg2rad(100)
#define DAMAGE_LASER_INI				1

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

#define PLAYER_CENTER_Y		0.5f

//Cambio malla
struct TCompRenderStaticMesh;

class ai_guard : public TCompBase, public aicontroller
{
	//Parameters
	float DIST_SQ_REACH_PNT = DIST_SQ_REACH_PNT_INI;
	float DIST_SQ_SHOT_AREA_ENTER = DIST_SQ_SHOT_AREA_ENTER_INI;
	float DIST_SQ_SHOT_AREA_LEAVE = DIST_SQ_SHOT_AREA_LEAVE_INI;
	float DIST_RAYSHOT = DIST_RAYSHOT_INI;
	float DIST_SQ_PLAYER_DETECTION = DIST_SQ_PLAYER_DETECTION_INI;
	float DIST_SQ_PLAYER_LOST = DIST_SQ_PLAYER_LOST_INI;
	float SPEED_WALK = SPEED_WALK_INI;
	float CONE_VISION = CONE_VISION_INI;
	float SPEED_ROT = SPEED_ROT_INI;
	float DAMAGE_LASER = DAMAGE_LASER_INI;

	//Handles & More
	CHandle myHandle;
	CHandle myParent;
	CHandle thePlayer;
	TCompTransform * getTransform();
	CEntity* getPlayer();

	//Cambio malla
	TCompRenderStaticMesh* actual_render = nullptr;
	CHandle pose_idle;
	CHandle pose_run;
	CHandle pose_shoot;

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
	std::vector<KeyPoint> keyPoints;
	int curkpt;
	VEC3 noisePoint;

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
	CHandle rayCastToPlayer(int types, float& distRay);
	void shootToPlayer();

	//from bombs
	float reduce_factor = 3.0f;
	float t_reduceStats_max = 15.0f;
	float t_reduceStats = 0.0f;

public:
	void SelectActionState();
	void NextActionState();
	void SeekPointState();
	void LookPointState();
	void WaitNextState();
	void ChaseState();
	void ShootState();
	void SoundDetectedState();
	void LookArroundState();
	void ShootingWallState();

	void Init() override;
	void init() { Init(); }
	void noise(const TMsgNoise& msg);

	//From bombs
	void reduceStats();
	void resetStats();
	void onMagneticBomb(const TMsgMagneticBomb& msg);
	void onPlayerDetected(const TMsgNoise& msg);

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
	void ChangePose(CHandle new_pos_h);

	____TIMER_DECLARE_VALUE_(timerShootingWall, 8)
};

#endif