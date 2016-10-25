#ifndef INC_FX_FOG_H_
#define INC_FX_FOG_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TRenderDepthFog : public TCompBasicFX {
	bool enabled;
	bool enabled_dist_calc;
	float hprev; // to fade
	float hnext; // to fade
	float rmin; // radius to fade
	float rmax; // radius to fade
	void init();
	void renderInMenu();

	void ApplyFX();

	void update(float dt);

	VEC3 m_position_point_distance;
	CHandle h_player;

	float alpha_orig;

	void SetPosition(VEC3 new_position);
	VEC3 GetPosition() const;
	void SetFloorHeight(float);
	void FadeHeight(float h_prev, float h_next, float rmin, float rmax);

	void EnableDistanceCalculation();
	void DisableDistanceCalculation();

	const char* getName() const {
		return "fog_depth";
	}
};

#endif
