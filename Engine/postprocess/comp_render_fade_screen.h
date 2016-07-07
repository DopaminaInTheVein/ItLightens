#ifndef INC_COMPONENT_RENDER_FADE_SCREEN_H_
#define INC_COMPONENT_RENDER_FADE_SCREEN_H_

#include "app_modules\postprocess.h"

// ------------------------------------
struct TFadeScreenFX : public IPostProcess{

	bool fade_in;
	bool fade_out;

	bool load(MKeyValue& atts);
	const CRenderTechnique *tech;

	float t_max_fade, t_max_fade_default;
	float t_curr_fade;

	void init();

	void renderInMenu();
	void update(float elapsed);
	void SetMaxTime(float new_time);
	void FadeIn();
	void FadeOut();

	void render();

	const char* getName() const {
		return "fade screen";
	}
};

#endif
