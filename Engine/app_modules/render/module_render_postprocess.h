#ifndef INC_MODULE_RENDER_POSTPROCESS_H_
#define INC_MODULE_RENDER_POSTPROCESS_H_

#include "app_modules/app_module.h"
#include "render/render.h"

#define FX_BLUR				"blur"
#define FX_FADESCREEN		"fade_screen"
#define FX_FADESCREEN_ALL	"fade_screen_all"
#define FX_HATCHING			"hatching"
#define FX_OUTLINE			"outline"
#define FX_DREAM_BORDER		"dream_border"
#define FX_MOTION_BLUR		"motion_blur"
#define FX_FOG_DEPTH		"fog_depth"
#define FX_ANTI_ALIASING	"anti_aliasing"
#define FX_SSAO				"ssao"

class TCompBasicFX;
class TRenderGlow;
class CTexture;

class CRenderPostProcessModule : public IAppModule {
	struct TKeyFX {
		TCompBasicFX* fx;
		int priority;
	};

	std::vector<TKeyFX> m_activated_end;		//posprocess to be executed at the end of the render.
	std::vector<TKeyFX> m_activated_ui_layer;	//postprocess to be executed before the UI.

	std::map<std::string, TCompBasicFX*> m_list_fx;

public:

	bool start();

	void ExecuteAllPendentFX();
	void ExecuteUILayerFX();

	template< typename TObj >
	TObj* GetFX(std::string name)
	{
		return dynamic_cast<TObj*>(m_list_fx[name]);
	}
	void ActivateFX(std::string name);
	void ActivateFX(TCompBasicFX* handle);

	void ActivateFXAtEnd(std::string name, int priority = 0);
	void ActivateFXAtEnd(TCompBasicFX* handle, int priority = 0);

	void RemoveActiveFX(std::string name);

	void RemoveActiveFX(TCompBasicFX * handle);

	void ActivateFXBeforeUI(std::string name, int priority = 0);
	void ActivateFXBeforeUI(TCompBasicFX* handle, int priority = 0);

	void AddFX(std::string name, TCompBasicFX* handle);

	bool isActive(std::string);

	static bool sortByPriority(const TKeyFX &k1, const TKeyFX &k2);

	void stop();
	bool forcedUpdate() override { return true; }
	void update(float dt);
	void init() {
		//nothing to do
	}
	void renderInMenu();

	const char* getName() const {
		return "postprocess module";
	}
};

extern CRenderPostProcessModule* render_fx;

#define GET_FX(var, type, name) type * var = render_fx->GetFX<type>(name);

#endif