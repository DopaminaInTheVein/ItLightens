#ifndef INC_COMPONENTS_MSGS_H_
#define INC_COMPONENTS_MSGS_H_

#include "handle/msgs.h"
#include "components/entity_tags.h"
#include "logic/damage.h"
#include "logic/polarity.h"
#include "skeleton/ik.h"

struct TMsgEntityCreated {
	DECLARE_MSG_ID();
};

class CPrefabCompiler;
struct TMsgPreload {
	CPrefabCompiler * comps;
	DECLARE_MSG_ID();
};
struct TMsgAwake {
	DECLARE_MSG_ID();
};

// Attach to skeleton bone
struct TMsgAttach {
	ClHandle handle;
	std::string bone_name;
	bool save_local_tmx;
	VEC3 offset;
	DECLARE_MSG_ID();
};

// Subscribe an IK bone solver
struct TMsgSetIKSolver {
	ClHandle handle;
	IK::bone_solver function;
	std::string bone_name;
	bool enable;
	float time;
	DECLARE_MSG_ID();
};

// Notify Player grabbed object hit
struct TMsgGrabHit {
	VEC3 * points;
	VEC3 * normals;
	VEC3 * impulses;
	float * separations;
	int npoints;
	DECLARE_MSG_ID();
};

// Notify object contact
struct TMsgContact {
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
	std::string source;
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
	ClHandle camera;
	DECLARE_MSG_ID();
};

enum PLAYER_TYPE {
	PLAYER
	, MOLE
	, SCIENTIST
};
struct TMsgSetTarget {
	ClHandle target;
	PLAYER_TYPE who;
	DECLARE_MSG_ID();
};

struct TMsgGetWhoAmI {
	PLAYER_TYPE who;
	DECLARE_MSG_ID();
};

struct TMsgSetPlayer {
	ClHandle player;
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
	std::string guard_name;
	DECLARE_MSG_ID();
};

struct TMsgCanDoActions {
	bool enabled;
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
	ClHandle generator; // empty --> out of range
	DECLARE_MSG_ID();
};

struct TMsgCanRechargeDrone {
	bool range;
	ClHandle han;
	DECLARE_MSG_ID();
};

struct TMsgCanNotRechargeDrone {
	bool range;
	ClHandle han;
	DECLARE_MSG_ID();
};

struct TMsgTriggerIn {
	ClHandle other;
	DECLARE_MSG_ID();
};

struct TMsgTriggerOut {
	ClHandle other;
	DECLARE_MSG_ID();
};

struct TMsgPolarize {
	ClHandle handle;
	bool range;
	DECLARE_MSG_ID();
};

struct TMsgPlayerPolarize {
	int type;
	DECLARE_MSG_ID();
};

struct TMsgSetTag {
	//uint32_t tag_id;
	std::string tag;
	bool add;
	DECLARE_MSG_ID();
};

struct TMsgSetAnim {
	std::vector<std::string> name;
	bool loop;
	std::vector<std::string> nextLoop;
	DECLARE_MSG_ID();
};

struct TMsgFollow {
	ClHandle follower;
	float speed;
	DECLARE_MSG_ID();
};

struct TMsgGetCullingViewProj {
	MAT44* view_proj;
	DECLARE_MSG_ID();
};

struct TMsgGetLocalAABB {
	AABB* aabb;
	DECLARE_MSG_ID();
};

struct TMsgSetActivable {
	bool activable;
	DECLARE_MSG_ID();
};

struct TMsgActivate {
	DECLARE_MSG_ID();
};

struct TMsgThrow {
	VEC3 dir;
	DECLARE_MSG_ID();
};

struct TMsgRepair {
	DECLARE_MSG_ID();
};

struct TMsgSetControllable {
	bool control;
	DECLARE_MSG_ID();
};
struct TMsgBoxHit {
	DECLARE_MSG_ID();
};
struct TMsgGuidedCamera {
	ClHandle guide;
	float speed;
	DECLARE_MSG_ID();
};
struct TMsgSetPolarity {
	pols polarity;
	DECLARE_MSG_ID();
};
struct TMsgSetLocked {
	bool locked;
	float speed;
	DECLARE_MSG_ID();
};
struct TMsgGetPolarity {
	pols polarity;
	DECLARE_MSG_ID();
};
struct TMsgGoAndLook {
	ClHandle target;
	std::string code_arrived;
	DECLARE_MSG_ID();
};

struct TMsgSetCharged {
	bool charged;
	DECLARE_MSG_ID();
};

struct TMsgIsCharged {
	bool charged;
	DECLARE_MSG_ID();
};
#endif
