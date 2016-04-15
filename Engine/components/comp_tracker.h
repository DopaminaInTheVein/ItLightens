#ifndef INC_TRACKER_H_
#define INC_TRACKER_H_

#define MAX_TRACK_POINTS 1000

#include "comp_msgs.h"
#include "comp_base.h"

struct TCompTracker : public TCompBase {

	VHandles followers;
	VEC3 positions[MAX_TRACK_POINTS];
	VEC3 orientations[MAX_TRACK_POINTS];
	int size;
	float longitude;
	float mSpeed = 1.f;

	//float	mEpsilon = 0.5f;	//distance margin
	//float	mSpeed;
	//VEC3	mMovement;
	//VEC3	mOrigin;
	//VEC3	mEnd;
	//int		mDirection = 1;

	void onCreate(const TMsgEntityCreated&);
	void update(float elapsed);
	bool load(MKeyValue& atts);
	void renderInMenu() {
		ImGui::Text("I am a platform!!");
	}
};


#endif