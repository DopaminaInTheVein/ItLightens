#ifndef INC_STATIC_BOMB_H_
#define	INC_STATIC_BOMB_H_

#include "components\comp_base.h"

#include "handle\handle.h"

class CStaticBomb : public TCompBase {

	const float t_explode = 5.0f;
	float t_waiting = 0.0f;

	CHandle myHandle;
	CHandle myParent;


public:
	void update(float elapsed);
	void Explode();

	void destroy() {
		myParent.destroy();
	}

	void SendMsg();
	CStaticBomb& CStaticBomb::operator=(CStaticBomb arg) { return arg; }
};

#endif