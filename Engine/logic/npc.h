#ifndef _NPC_INC
#define _NPC_INC

#include "bt.h"

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

	bool getPath(const VEC3& startPoint, const VEC3& endPoint);
	CEntity* frontCollisionIA(const VEC3 & npcPos, CHandle ownHandle);
	CEntity* frontCollisionBOX(const TCompTransform * transform, CEntity *  molePursuingBoxi);
	bool avoidBoxByLeft(CEntity * candidateE, const TCompTransform * transform);
	bool needsSteering(VEC3 npcPos, TCompTransform * transform, float rotation_speed, CHandle myHandle, CEntity * molePursuingBoxi = nullptr);

public:
	//Prueba
	int getPathDebug(const VEC3& startPoint, const VEC3& endPoint) {
		if (getPath(startPoint, endPoint)) return totalPathWpt;
		else return -1;
	}

	void initParent() {
		currPathWpt = totalPathWpt = 0;
		pathWpts.clear();
		if (state_ini != "") setCurrent(findNode(state_ini));
		else setCurrent(NULL);
	}
};

#endif