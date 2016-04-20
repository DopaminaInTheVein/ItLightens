#ifndef INC_COMPONENTS_MSGS_H_
#define INC_COMPONENTS_MSGS_H_

#include "handle/msgs.h"
#include "components/entity_tags.h"
#include "logic/damage.h"

struct TMsgEntityCreated {
	DECLARE_MSG_ID();
};

// Sent to all entities from a parsed file once all the entities
// in that file has been created. Used to link entities between them
struct TMsgEntityGroupCreated {
	VHandles* handles;
	DECLARE_MSG_ID();
};

struct TMsgBoxDestroyed {
	DECLARE_MSG_ID();
};

struct TMsgLeaveBox {
	DECLARE_MSG_ID();
};

struct TMsgDamage {
	float modif;
	DECLARE_MSG_ID();
};

struct TMsgDamageSpecific {
	Damage::DMG_TYPE type;
	bool actived;
	DECLARE_MSG_ID();
};

struct TMsgDie {
	DECLARE_MSG_ID();
};

struct TMsgDamageSave {
	float modif;
	DECLARE_MSG_ID();
};

struct TMsgStopDamage {
	DECLARE_MSG_ID();
};

struct TMsgSetDamage {
	float dmg;
	DECLARE_MSG_ID();
};

struct TMsgUnpossesDamage {
	DECLARE_MSG_ID();
};

struct TMsgSetCamera {
	CHandle camera;
	DECLARE_MSG_ID();
};

enum PLAYER_TYPE {
	PLAYER
	, MOLE
	, SPEEDY
	, SCIENTIST
};
struct TMsgSetTarget {
	CHandle target;
	PLAYER_TYPE who;
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

struct TMsgOverCharge {
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
	bool range;
	DECLARE_MSG_ID();
};

struct TMsgCanRec {
	bool range;
	DECLARE_MSG_ID();
};

struct TMsgTriggerIn {
	CHandle other;
	DECLARE_MSG_ID();
};

struct TMsgTriggerOut {
	CHandle other;
	DECLARE_MSG_ID();
};

struct TMsgPolarize {
	VEC3 origin;
	int pol;
	bool range;
	DECLARE_MSG_ID();
};

struct TMsgPlayerPolarize {
	int type;
	DECLARE_MSG_ID();
};

struct TMsgAddTag {
	uint32_t tag_id;
	DECLARE_MSG_ID();
};

struct TMsgSetAnim {
	std::string name;
	DECLARE_MSG_ID();
};

struct TMsgFollow {
	CHandle follower;
	DECLARE_MSG_ID();
};
#endif
