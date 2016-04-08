#ifndef INC_COMPONENTS_MSGS_H_
#define INC_COMPONENTS_MSGS_H_

#include "handle/msgs.h"
#include "components/entity_tags.h"

// Sent to the entity when all components of the entity have
// been added after parsing them in the xml. Each component
// can have access to all other components in the same entity
struct TMsgEntityCreated {
  DECLARE_MSG_ID();
};

// Sent to all entities from a parsed file once all the entities
// in that file has been created. Used to link entities between them
struct TMsgEntityGroupCreated {
  VHandles* handles;
  DECLARE_MSG_ID();
};

// Example dummy msg
struct TMsgDamage {
  VEC3    source;
  CHandle sender;
  float   points;
  DECLARE_MSG_ID();
};

struct TMsgSetTarget {
  CHandle target;
  DECLARE_MSG_ID();
};

struct TMsgAddTag {
  uint32_t tag_id;
  DECLARE_MSG_ID();
};


#endif
