#ifndef INC_WATER_H_
#define INC_WATER_H_

#include "aicontroller.h"
#include "sbb.h"

#include "components\comp_base.h"
#include "handle\handle.h"
#include "components\comp_msgs.h"

class CEntity;

template< class TObj >
class CObjectManager;

class water_controller : public aicontroller, public TCompBase {
	std::map<int, std::string> out;

	CObjectManager<water_controller>	*om = nullptr;
	CHandle								 myHandle;
	CHandle								 myParent;
	CEntity								*myEntity = nullptr;

	CHandle								 player;

	int id_water = 0;
	std::string full_name = "";

	int permanent_water_damage = 100000;
	int dropped_water_damage = 150;

	float permanent_max_ttl = -1.f;				//in seconds
	float dropped_max_ttl = 10.f;					//in seconds

	float damage_radius = 25.f;

	int water_type;
	int damage;
	float ttl;

	bool dead = false;

	void updateTTL();
	void tryToDamagePlayer();

public:

	static int id_curr_max_waters;

	enum {
		PERMANENT = 0,
		DROPPED,
	};

	water_controller() {}
	void Init() override;
	void init() { Init(); }

	void update(float elapsed);

	void SetHandleMeInit();
	void SetMyEntity();

	void onSetWaterType(const TMsgSetWaterType& msg);

	void Idle();
	void Die();
	void Dead();

	void renderInMenu();

	water_controller& water_controller::operator=(water_controller arg) { return arg; }
};

#endif