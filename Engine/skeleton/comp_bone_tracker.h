#ifndef INC_COMPONENT_BONE_TRACKER_H_
#define INC_COMPONENT_BONE_TRACKER_H_

#include "components/comp_base.h"
#include "handle/handle.h"
#include "components/comp_msgs.h"

struct TCompBoneTracker : public TCompBase {
	CHandle  h_entity;
	uint32_t bone_id;
	char     bone_name[64];
	char     entity_name[64];

	//Local transform when was attached
	MAT44 local_tmx;
	bool local_tmx_saved;
	/*GET_MY(tmx, TCompTransform);
	CalVector abs_front = Engine2Cal(tmx->getFront());
	CalVector abs_left = Engine2Cal(tmx->getLeft());
	CalQuaternion abs_rot_c = bone_c->getRotationAbsolute();
	CalQuaternion inv_abs_rot_c = abs_rot_c;
	inv_abs_rot_c.invert();
	*/
	void renderInMenu();
	bool load(MKeyValue& atts);
	void update(float dt);
	void onGroupCreated(const TMsgEntityGroupCreated& msg);
	void onAttach(const TMsgAttach& msg);
	void onAttachWithOffset(const TMsgAttach& msg);
};

#endif