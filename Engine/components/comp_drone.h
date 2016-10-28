#ifndef INC_DRON_H_
#define INC_DRON_H_

#include "comp_base.h"
#include "comp_trigger.h"

class TCompTransform;
class TCompPhysics;
struct TCompLife;

struct TCompDrone : public TTrigger {
	TCompTransform* transform;
	TCompPhysics* physics;
	TCompLife* life;

	std::vector<VEC3> wpts;
	std::vector<float> waitTimes;
	int curWpt;
	float speed = 1.0f;
	float fallingSpeed = 3.0f;
	float timeToWait = 0.f;
	float mEpsilon = 1.1f;
	bool playerInDistance = false;
	bool sciInDistance = false;
	bool espatllat = false;

	std::string self_ilum_front;
	std::string self_ilum_back;

	// sound parameters
	std::string name;
	std::string moving_sound_event = "event:/OnDroneMoving";
	std::string static_sound_event = "event:/OnDroneStatic";
	float volume = 0.5f;
	bool moving = false;

	VEC3 final_pos;

	void onCreate(const TMsgEntityCreated&);
	void onRecharge(const TMsgActivate &);
	void onRepair(const TMsgRepair &);
	void CanRechargeDrone(bool new_range);
	void CanNotRechargeDrone(bool new_range);
	void CanRepairDrone(CHandle sci, bool new_range);

	void onTriggerInside(const TMsgTriggerIn& msg);
	void onTriggerEnter(const TMsgTriggerIn& msg);
	void onTriggerExit(const TMsgTriggerOut& msg);
	void mUpdate(float dt) {}

	bool SetMyBasicComponents();
	void update(float elapsed);
	void moveToNext(float elapsed);
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	void fixedUpdate(float elapsed);
	void renderInMenu() {
		ImGui::Text("I am a drone!!");
	}
};

#endif