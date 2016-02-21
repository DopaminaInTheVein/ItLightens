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

struct TMsgBeaconToRemove {
	VEC3 pos_beacon;
	std::string name_beacon;
	DECLARE_MSG_ID();
};

struct TMsgBeaconEmpty {
	VEC3 pos_beacon;
	std::string name_beacon;
	DECLARE_MSG_ID();
};

#endif
