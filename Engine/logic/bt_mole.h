#ifndef _BT_MOLE_INC
#define _BT_MOLE_INC

#include "mcv_platform.h"
#include "bt_poss.h"
#include "sbb.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/comp_name.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include "resources/resources_manager.h"
#include "components/comp_render_static_mesh.h"
#include "resources/resources_manager.h"
#include "render/mesh.h"
#include "render/static_mesh.h"
#include "render/mesh.h"
#include "render/shader_cte.h"
#include "utils/utils.h"
#include "utils/XMLParser.h"
#include <vector>
#include <chrono>
#include <windows.h>

class bt_mole : public bt_poss, public TCompBase {
	float speed = 2.f;
	float rotation_speed = deg2rad(200);
	float distMaxToBox = 15.0f;

	int towptbox;
	int towptleave;

	bool carryingBox = false;

	vector<VEC3> fixedWpts;
	int curwpt;

	CObjectManager<bt_mole> * om = nullptr;
	CHandle myHandle;
	CHandle myParent;
	CEntity *myEntity = nullptr;

	TCompTransform* transform;

	void SetMyEntity();

	CEntity * getEntityPointer(int i) {
		CHandle han = SBB::readHandlesVector("wptsBoxes")[i];
		CEntity * ent = han;
		return ent;
	}

	CEntity* getMyEntity() {
		CHandle me = CHandle(this);
		return me.getOwner();
	}
	bool isBoxAtLeavePoint(VEC3 posBox);

	//Cambio Malla
	TCompRenderStaticMesh* mesh;
	string pose_idle_route;
	string pose_run_route;
	string pose_jump_route;
	string pose_box_route;
	string pose_wall_route;

public:
	void Init();
	void update(float elapsed);
	//conditions
	bool checkBoxes();
	//actions
	int actionSeekBoxWpt();
	int actionNextBoxWpt();
	int actionGrabBox();
	int actionCarryFindBoxWpt();
	int actionCarryNextBoxWpt();
	int actionCarrySeekBoxWpt();
	int actionUngrabBox();
	int actionSeekWpt();
	int actionNextWpt();

	bool load(MKeyValue& atts);

	void _actionBeforePossession();
	void _actionWhenStunt();

	bool aimToTarget(VEC3 target);
	void moveFront(float movement_speed);

	//Cambio Malla
	void ChangePose(string new_pose_route);
};

#endif