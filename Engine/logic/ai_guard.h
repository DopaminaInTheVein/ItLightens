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
#define SPEED_WALK_INI					6
#define CONE_VISION_INI					deg2rad(45)
#define SPEED_ROT_INI					deg2rad(100)
#define DAMAGE_LASER_INI				3

#define ST_NEXT_ACTION		"next_action"
#define ST_SEEK_POINT		"seek_point"
#define ST_WAIT_NEXT		"wait_next"
#define ST_LOOK_POINT		"look_point"
#define ST_CHASE			"chase"
#define ST_SHOOT			"shoot"
#define ST_SOUND_DETECTED	"sound_detected"
#define ST_LOOK_ARROUND		"look_arround"

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

	//Debug
	____TIMER_DECLARE_(timerDebug, 2.0f);

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

	std::vector<KeyPoint> keyPoints;
	int curkpt;
	VEC3 noisePoint;

	//Aux actions
	void goTo(const VEC3& dest);
	void goForward(float stepForward);
	bool turnTo(VEC3 dest);

	//Aux checks
	bool playerVisible();

	//Times and similars
	float timeWaiting;
	float deltaYawLookingArround;
	void resetTimes();

	//Raycast
	CHandle rayCastToFront(char types, float& distRay);

public:
	void NextActionState();
	void SeekPointState();
	void LookPointState();
	void WaitNextState();
	void ChaseState();
	void ShootState();
	void SoundDetectedState();
	void LookArroundState();

	void Init() override;
	void init() { Init(); }
	void noise(const TMsgNoise& msg);

	void update(float dt) { Recalc(); }
	void render();
	void renderInMenu();
	bool load(MKeyValue& atts);
};

#endif