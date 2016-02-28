#ifndef INC_MAG_BOMB_H_
#define	INC_MAG_BOMB_H_

#include "components\comp_base.h"
#include "logic\aicontroller.h"

#include "handle\handle.h"

class CEntity;

class CMagneticBomb : public TCompBase, public aicontroller {

	const float t_explode = 5.0f;
	float t_waiting = 0.0f;
	float x_local = 0.0f;

	CHandle myHandle;
	CHandle myParent;

	CEntity *myEntity;

	void SetMyEntity();

public:
	void Init();
	void GoingUp();
	void GoingDown();
	void CountDown();
	void Explode();



	void UpdatePosition();
	void update(float elapsed);
	void destroy() {
		myParent.destroy();
	}

	void SendMsg();
	CMagneticBomb& CMagneticBomb::operator=(CMagneticBomb arg) { return arg; }
};

#endif