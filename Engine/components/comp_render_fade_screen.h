#ifndef INC_COMPONENT_RENDER_FADE_SCREEN_H_
#define INC_COMPONENT_RENDER_FADE_SCREEN_H_

#include "comp_base.h"

// ------------------------------------
struct TCompFadeScreen : public TCompBase {
	bool fade_in;
	bool fade_out;

	bool load(MKeyValue& atts);
	const CRenderTechnique *tech;

	float t_max_fade, t_max_fade_default;
	float t_curr_fade;

	void onCreate(const TMsgEntityCreated&);

	void renderInMenu();
	void update(float elapsed);
	void SetMaxTime(float new_time);
	void FadeIn();
	void FadeOut();

	void render();
};

#endif
