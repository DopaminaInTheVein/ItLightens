#ifndef INC_FX_FADE_SCREEN_H_
#define INC_FX_FADE_SCREEN_H_

#include "app_modules\render\comp_basic_fx.h"

// ------------------------------------
struct TFadeScreen : public TCompBasicFX {
	bool fade_in;
	bool fade_out;

	bool load(MKeyValue& atts);

	float t_max_fade, t_max_fade_default;
	float t_curr_fade;

	void init();

	void renderInMenu();
	void update(float elapsed);
	void SetMaxTime(float new_time);
	void FadeIn();
	void FadeOut();

	void ApplyFX();

	const char* getName() const {
		return "fade_screen";
	}
};

#endif
