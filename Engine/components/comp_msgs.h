#ifndef INC_COMPONENTS_MSGS_H_
#define INC_COMPONENTS_MSGS_H_

#include "handle/msgs.h"

struct TMsgEntityCreated {
  DECLARE_MSG_ID();
};

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

#endif
