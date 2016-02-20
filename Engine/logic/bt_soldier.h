#ifndef _BT_SOLDIER_INC
#define _BT_SOLDIER_INC


#include "bt.h"


class bt_soldier:public bt
	{
	public:
		void create(string, TEntity* new_entity);

		int actionReset();
		int actionChase();
		int actionCombat();
		int actionOLeft();
		int actionORight();
		int actionFollowWpt();
		int actionNextWpt();

		bool bt_soldier::conditionReset();
		bool bt_soldier::conditionFight();
		bool bt_soldier::conditionChase();
		bool bt_soldier::conditionPatrol();
	};

#endif