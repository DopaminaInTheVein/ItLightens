#ifndef INC_COMPONENTS_MSGS_H_
#define INC_COMPONENTS_MSGS_H_

#include "handle/msgs.h"

struct TMsgEntityCreated {
	DECLARE_MSG_ID();
};

enum DMGTYPE {
	UNKNOWN
	, ENERGY_DECREASE
	, LASER
	, WATER
};
struct TMsgDamage {
	VEC3    source;
	CHandle sender;
	DMGTYPE dmgType;
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

struct TMsgSetWaterType {
	int type;
	DECLARE_MSG_ID();
};

struct TMsgBeaconToRemove {
	VEC3 pos_beacon;
	std::string name_beacon;
	DECLARE_MSG_ID();
};

struct TMsgBeaconEmpty {
	VEC3 pos;
	std::string name;
	DECLARE_MSG_ID();
};

struct TMsgWBEmpty {
	VEC3 pos;
	std::string name;
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

struct TMsgWBTakenByPlayer {
	std::string name;
	DECLARE_MSG_ID();
};

struct TMsgStaticBomb {
	VEC3 pos;
	float r;
	DECLARE_MSG_ID();
};

struct TMsgMagneticBomb {
	VEC3 pos;
	float r;
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

struct TMsgAISetStunned {
	bool stunned;
	DECLARE_MSG_ID();
};

struct TMsgWirePass {
	VEC3 dst;
	DECLARE_MSG_ID();
};

struct TMsgCanRec {
	DECLARE_MSG_ID();
};

#endif
