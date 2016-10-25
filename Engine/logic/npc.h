#ifndef _NPC_INC
#define _NPC_INC

#include "bt.h"

#define PLAYER_CENTER_Y					0.5f

// Implementation of the behavior tree
// uses the BTnode so both work as a system
// tree implemented as a map of btnodes for easy traversal
// behaviours are a map to member function pointers, to
// be defined on a derived class.
// BT is thus designed as a pure abstract class, so no
// instances or modifications to bt / btnode are needed...

class npc : public bt
{
protected:
	vector<VEC3> pathWpts;
	int currPathWpt;
	int totalPathWpt;

	float SPEED_WALK;
	float SPEED_ROT;
	float DIST_REACH_PNT;

	// talk management
	float talk_time = 0.f;
	float MAX_TIME_TALK = rand() % 60;

	// stuck management
	float MAX_STUCK_TIME;
	float UNSTUCK_DISTANCE;
	float stuck_time = 0.f;
	bool stuck = false;
	bool reoriented = false;
	bool path_found = false;
	int direction = 0;

	VEC3 unstuck_target;
	VEC3 prev_unstuck_target;
	float PREV_UNSTUCK_DISTANCE;

	VEC3 last_position;
	btnode* action_when_stuck = NULL;

	bool getPath(const VEC3& startPoint, const VEC3& endPoint);
	CEntity* frontCollisionIA(const VEC3 & npcPos, CHandle ownHandle);
	CEntity* frontCollisionBOX(const TCompTransform * transform, CEntity *  molePursuingBoxi);
	bool avoidBoxByLeft(CEntity * candidateE, const TCompTransform * transform);
	bool needsSteering(VEC3 npcPos, TCompTransform * transform, float rotation_speed, CHandle myHandle, CEntity * molePursuingBoxi = nullptr);

	//Add npc states
	void addNpcStates(std::string);

	//Update stuck
	void updateStuck();

	//Aux actions
	void readNpcIni(std::map<std::string, float>&);
	void goTo(const VEC3& dest);
	void goForward(float stepForward);
	bool turnYaw(float delta_yaw, float angle_epsilon = 5.f);
	bool turnToYaw(float target_yaw);
	bool turnTo(VEC3 dest, bool wide = false);
	void computeUnstuckTarget();

	virtual TCompTransform * getTransform() = 0;
	virtual void changeCommonState(std::string) = 0;
	virtual CHandle getParent() = 0;
	virtual TCompCharacterController * getCC() = 0;

public:
	//Conditions
	bool npcStuck();

	//Actions
	int actionUnstuckTurn();
	int actionUnstuckMove();

	//Talk
	void updateTalk(string npc_name, CHandle handle);

	//Prueba
	int getPathDebug(const VEC3& startPoint, const VEC3& endPoint) {
		if (getPath(startPoint, endPoint)) return totalPathWpt;
		else return -1;
	}

	std::vector<VEC3> getPathWpts() {
		return pathWpts;
	}

	void initParent() {
		currPathWpt = totalPathWpt = 0;
		pathWpts.clear();
		if (state_ini != "") setCurrent(findNode(state_ini));
		else setCurrent(NULL);
	}
};

#endif