#ifndef INC_MAG_BOMB_H_
#define	INC_MAG_BOMB_H_

#include "components\comp_base.h"
#include "logic\aicontroller.h"

#include "handle\handle.h"

class CEntity;

class CMagneticBomb : public TCompBase {
	float t_explode = 5.0f;
	float t_waiting = 0.0f;
	float x_local = 0.0f;

	ClHandle myHandle;
	ClHandle myParent;

	CEntity *myEntity;

	void SetMyEntity();

protected:
	// the states, as maps to functions
	static map<string, statehandler> statemap;

public:

	void Init();
	void CountDown();
	void Explode();

	void UpdatePosition();
	void update(float elapsed);
	void destroy() {
		myParent.destroy();
	}

	void SendMsg();
};

#endif