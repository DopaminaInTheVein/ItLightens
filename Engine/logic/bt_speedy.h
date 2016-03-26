#ifndef _BT_SPEEDY_INC
#define _BT_SPEEDY_INC

#include "mcv_platform.h"
#include "bt_poss.h"
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

class bt_speedy : public bt_poss, public TCompBase {

	CObjectManager<bt_speedy> * om = nullptr;
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

	float speed = 4.f;
	float dash_speed = 20.f;
	float rotation_speed = deg2rad(200);
	float max_dash_player_distance = 70.f;
	// timers in seconds
	float dash_timer_reset = 10.f;
	float drop_water_timer_reset = 3.f;
	// probabilities
	int random_wpt = -1;

	string water_static_mesh = "static_meshes/water.static_mesh";

	CEntity* getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}

	TCompRenderStaticMesh* mesh;
	string pose_idle_route;
	string pose_run_route;
	string pose_jump_route;
	string last_pose = "";

	public:
		void Init();
		void update(float elapsed);
		//conditions
		bool dashReady();
		//actions
		int actionSeekWpt();
		int actionNextWpt();
		int actionDashPoint();
		int actionDashNewPoint();
		int actionDashPlayer();

		bool load(MKeyValue& atts);

		void onSetPlayer(const TMsgSetPlayer& msg);

		void SetMyEntity();
		bool aimToTarget(VEC3 target);
		void moveFront(float speed);
		bool dashToTarget(VEC3 target);
		void updateDashTimer();
		void resetDashTimer();
		void updateDropWaterTimer();
		void resetDropWaterTimer();

		//Cambio Malla
		void ChangePose(string new_pose_route);
	};

#endif