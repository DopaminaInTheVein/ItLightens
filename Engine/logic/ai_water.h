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
	static std::map<int, std::string> out;

	CObjectManager<water_controller>	*om = nullptr;
	CHandle								 myHandle;
	CHandle								 myParent;
	CEntity								*myEntity = nullptr;

	CHandle								 player;

	int id_water = 0;
	std::string full_name = "";

	// Main attributes
	float permanent_water_damage;
	float dropped_water_damage;
	float permanent_max_ttl;				//in seconds
	float dropped_max_ttl;				//in seconds
	float damage_radius;

	int water_type;
	int damage;
	float ttl;

	bool dead = false;

	void updateTTL();
	void tryToDamagePlayer();

protected:
	// the states, as maps to functions
	static map<string, statehandler>statemap;

public:

	static int id_curr_max_waters;

	enum {
		PERMANENT = 0,
		DROPPED,
	};

	map<string, statehandler>* getStatemap() override {
		return &statemap;
	}

	water_controller() {}
	void Init() override;
	void init() { Init(); }
	void readIniFileAttr();

	void update(float elapsed);

	void SetHandleMeInit();
	void SetMyEntity();

	void onSetWaterType(const TMsgSetWaterType& msg);

	void Idle();
	void Die();
	void Dead();

	void renderInMenu();
};

#endif