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

struct TMsgSetCamera {
	CHandle camera;
	DECLARE_MSG_ID();
};

struct TMsgSetTarget {
	CHandle target;
	DECLARE_MSG_ID();
};

struct TMsgSetPlayer {
	CHandle player;
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

struct TMsgBeaconBusy {
	VEC3 pos;
	bool* reply;
	DECLARE_MSG_ID();
};

struct TMsgBeaconTakenByPlayer {
	std::string name;
	DECLARE_MSG_ID();
};

struct TMsgStaticBomb {
	float x_max;
	float z_max;
	float x_min;
	float z_min;
	DECLARE_MSG_ID();
};

struct TMsgMagneticBomb {
	float x_max;
	float z_max;
	float x_min;
	float z_min;
	DECLARE_MSG_ID();
};

struct TMsgNoise {
	VEC3 source;
	float intensity;
	DECLARE_MSG_ID();
};

struct TMsgControllerSetEnable {
	bool enabled;
	DECLARE_MSG_ID();
};

struct TMsgPossessionLeave {
	VEC3 npcPos;
	VEC3 npcFront;
	DECLARE_MSG_ID();
};

struct TMsgAISetPossessed {
	bool possessed;
	DECLARE_MSG_ID();
};

#endif
