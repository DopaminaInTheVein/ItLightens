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
	CHandle handle;
	std::string bone_name;
	bool save_local_tmx;
	VEC3 offset;
	DECLARE_MSG_ID();
};

// Subscribe an IK bone solver
struct TMsgSetIKSolver {
	CHandle handle;
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

struct TMsgHierarchySolver {
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
	CHandle camera;
	DECLARE_MSG_ID();
};

enum PLAYER_TYPE {
	PLAYER
	, MOLE
	, SCIENTIST
	, GUARD
};
struct TMsgSetTarget {
	CHandle target;
	PLAYER_TYPE who;
	DECLARE_MSG_ID();
};

struct TMsgGetWhoAmI {
	bool action_flag = false;
	PLAYER_TYPE who;
	std::string who_string;
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
	std::vector<std::string> guard_names;
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
	CHandle npc;
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
	CHandle generator; // empty --> out of range
	DECLARE_MSG_ID();
};

struct TMsgCanRechargeDrone {
	bool range;
	CHandle han;
	DECLARE_MSG_ID();
};

struct TMsgCanNotRechargeDrone {
	bool range;
	CHandle han;
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

struct TMsgTriggerUnpossess {
	CHandle other;
	DECLARE_MSG_ID();
};

struct TMsgPolarize {
	CHandle handle;
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
	CHandle follower;
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
struct TMsgSetOnlySense {
	bool sense;
	DECLARE_MSG_ID();
};
struct TMsgBoxHit {
	DECLARE_MSG_ID();
};
struct TMsgGuidedCamera {
	CHandle guide;
	float speed;
	bool start;
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
	CHandle target;
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

//GUI messages
struct TMsgOverButton {
	CHandle button;
	bool is_over;
	DECLARE_MSG_ID();
};
struct TMsgClicked {
	DECLARE_MSG_ID();
};
struct TMsgGuiSetListener {
	CHandle listener;
	std::string event_name;
	DECLARE_MSG_ID();
};
struct TMsgGuiNotify {
	CHandle notifier;
	std::string event_name;
	DECLARE_MSG_ID();
};
struct TMsgLanguageChanged {
	DECLARE_MSG_ID();
};
struct TMsgControlsChanged {
	DECLARE_MSG_ID();
};
struct TMsgDifficultyChanged {
	DECLARE_MSG_ID();
};
#endif
