#ifndef INC_DRON_H_
#define INC_DRON_H_

#include "comp_base.h"

class TCompTransform;
class TCompPhysics;


struct TCompDrone : public TCompBase {
	TCompTransform* transform;
	TCompPhysics* physics;


	std::vector<VEC3> wpts;
	std::vector<float> waitTimes;
	int curWpt;
	float speed = 2.0f;
	float timeToWait = 0.f;
	float mEpsilon = .1f;


	std::string self_ilum_front;
	std::string self_ilum_back;

	VEC3 final_pos;

	void onCreate(const TMsgEntityCreated&);
	bool SetMyBasicComponents();
	void update(float elapsed);
	void moveToNext(float elapsed);
	bool load(MKeyValue& atts);
	void fixedUpdate(float elapsed);
	void renderInMenu() {
		ImGui::Text("I am a drone!!");
	}
};


#endif