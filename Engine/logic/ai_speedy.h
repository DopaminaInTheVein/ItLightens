#ifndef _AI_SPEEDY_
#define _AI_SPEEDY_

#include "mcv_platform.h"
#include "ai_poss.h"
#include "aicontroller.h"
#include "sbb.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "utils/utils.h"
#include "utils/XMLParser.h"
#include <vector>
#include <chrono>
#include <windows.h>

class ai_speedy : public ai_poss, public TCompBase {
	CObjectManager<ai_speedy> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CEntity *myEntity = nullptr;

	CHandle player;

	TCompTransform* transform;
	TCompTransform* player_transform;

	vector<VEC3> fixedWpts;
	int curwpt;

	float dash_timer;
	bool dash_ready;
	VEC3 dash_target;

	const float speed = 2.f;
	const float dash_speed = 20.f;
	const float rotation_speed = deg2rad(200);
	// timer in seconds
	const int dash_timer_reset = 5;

	const int dash_to_point_chance = 2;
	const int dash_to_new_point_chance = 2;
	const int dash_to_player_chance = 2;

	CEntity* getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}
public:
	void IdleState();
	void NextWptState();
	void SeekWptState();
	void DashToPlayerState();
	void DashToPointState();
	void DashToNewPointState();

	void Init();
	void init() { IdleState(); }
	void idle() { IdleState(); }
	void update(float elapsed);
	bool load(MKeyValue& atts);

	void onSetPlayer(const TMsgSetPlayer& msg);

	void SetMyEntity();
	string decide_next_action();
	bool aimToTarget(VEC3 target);
	void moveFront(float speed);
	bool dashToTarget(VEC3 target);
	void updateDashTimer();
	void resetDashTimer();

	//Overload function for handler_manager
	ai_speedy& ai_speedy::operator=(ai_speedy arg) { return arg; }
};

#endif