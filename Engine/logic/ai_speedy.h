#ifndef _AI_SPEEDY_
#define _AI_SPEEDY_

#include "mcv_platform.h"
#include "ai_poss.h"
#include "aicontroller.h"
#include "sbb.h"
#include "ai_water.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "components/entity_tags.h"
#include "components/comp_render_static_mesh.h"
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

extern TTagsManager tags_manager;

//Cambio malla
struct TCompRenderStaticMesh;

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
	float drop_water_timer;
	bool dash_ready;
	bool drop_water_ready;
	VEC3 dash_target;

	const float speed = 2.f;
	const float dash_speed = 20.f;
	const float rotation_speed = deg2rad(200);
	const float max_dash_player_distance = 70.f;
	// timers in seconds
	const float dash_timer_reset = 10.f;
	const float drop_water_timer_reset = 3.f;
	// probabilities
	const int dash_to_point_chance = 2;
	const int dash_to_new_point_chance = 2;
	const int dash_to_player_chance = 2;

	const string water_static_mesh = "static_meshes/water.static_mesh";

	CEntity* getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}

	//Cambio malla
	TCompRenderStaticMesh* actual_render = nullptr;
	CHandle pose_idle;
	CHandle pose_run;
	CHandle pose_jump;

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
	void updateDropWaterTimer();
	void resetDropWaterTimer();

	//Cambio Malla
	void ChangePose(CHandle new_pos_h);

	//Overload function for handler_manager
	ai_speedy& ai_speedy::operator=(ai_speedy arg) { return arg; }
};

#endif