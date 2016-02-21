#ifndef INC_AIC_GUARD_H_
#define INC_AIC_GUARD_H_

#include "aicontroller.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include <vector>

class ai_guard : public TCompBase, public aicontroller
{
	//Handles & More
	CHandle myHandle;
	CHandle myParent;
	TCompTransform * getTransform();

	//KeyPoints
	enum KptType { Seek, Look };
	struct KeyPoint {
		KptType type;
		VEC3 pos;
		float time;
		KeyPoint() : type(KptType::Seek), pos(VEC3(0, 0, 0)), time(0) {};
		KeyPoint(VEC3 p) : type(KptType::Seek), pos(p), time(0) {};
		KeyPoint(KptType t, VEC3 p) : type(t), pos(p), time(0) {};
		KeyPoint(KptType t, VEC3 p, float temps) : type(t), pos(p), time(temps) {};
	};
	static map<string, KptType > kptTypes;

	std::vector<KeyPoint> keyPoints;
	int curkpt;
	VEC3 noisePoint;

	//Aux actions
	void goTo(const VEC3& dest);
	void goForward(float stepForward);
	bool turnTo(VEC3 dest);

	//Aux checks
	bool playerVisible();

	//Times and similars
	float timeWaiting;
	float deltaYawLookingArround;
	void resetTimes();

public:
	void NextActionState();
	void SeekPointState();
	void LookPointState();
	void WaitNextState();
	void ChaseState();
	void ShootState();
	void SoundDetectedState();
	void LookArroundState();

	void Init() override;
	void init() { Init(); }
	void noise(const TMsgNoise& msg);

	void update(float dt) { Recalc(); }
	void render();
	void renderInMenu();
	bool load(MKeyValue& atts);
};

#endif