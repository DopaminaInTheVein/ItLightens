#ifndef _AI_MOLE_
#define _AI_MOLE_

#include "mcv_platform.h"
#include "aicontroller.h"
#include "ai_poss.h"
#include "sbb.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/comp_name.h"
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

class ai_mole : public ai_poss, public TCompBase {
	int towptbox;
	int towptleave;

	bool carryingBox = false;

	CHandle myHandle;
	CHandle myParent;

	TCompTransform * getEntityTransform() {
		CEntity * ent = myParent;
		return ent->get<TCompTransform>();
	}

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

public:
	void IdleState();
	void SeekWptState();
	void NextWptState();
	void OrientToWptState();
	void GrabState();
	void SeekWptCarryState();
	void NextWptCarryState();
	void OrientToCarryWptState();
	void UnGrabState();

	void _actionBeforePossession();
	void actionStunt();

	void Init() override;
	void update(float dt) { Recalc(); }
	void init() { Init(); }
	void idle() { IdleState(); }
};

#endif