#ifndef _AIC_PATROL
#define _AIC_PATROL

#include "aicontroller.h"

class aic_patrol : public aicontroller
{
public:

	vector<VEC3> wpts;
	int curwpt;

	void IdleState();
	void SeekWptState();
	void NextWptState();

	void Init(TEntity* new_entity);
};

#endif