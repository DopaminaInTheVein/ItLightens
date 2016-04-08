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
  void renderInMenu();
  bool load(MKeyValue& atts);
  void update( float dt );
  void onGroupCreated(const TMsgEntityGroupCreated& msg);
};

#endif

