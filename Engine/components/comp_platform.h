#ifndef INC_PLATFORM_H_
#define INC_PLATFORM_H_

#include "comp_base.h"

struct TCompPlatform : public TCompBase {

	float	mEpsilon = 0.5f;	//distance margin
	float	mSpeed;
	VEC3	mMovement;
	VEC3	mOrigin;
	VEC3	mEnd;
	int		mDirection = 1;

	void onCreate(const TMsgEntityCreated&);
	void update(float elapsed);
	bool load(MKeyValue& atts);
	void renderInMenu() {
		ImGui::Text("I am a platform!!");
	}
};


#endif