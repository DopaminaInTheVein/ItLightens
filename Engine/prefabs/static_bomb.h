#ifndef INC_STATIC_BOMB_H_
#define	INC_STATIC_BOMB_H_

#include "components\comp_base.h"

#include "handle\handle.h"

class CStaticBomb : public TCompBase {

	float t_explode = 5.0f;
	float t_waiting = 0.0f;

	float rad = 15.0f;

	CHandle myHandle;
	CHandle myParent;


public:
	void update(float elapsed);
	void Explode();
	void toExplode();

	void destroy() {
		myParent.destroy();
	}

	void SendMsg();
};

#endif