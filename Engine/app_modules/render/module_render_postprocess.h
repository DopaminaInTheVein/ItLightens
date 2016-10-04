#ifndef INC_MODULE_RENDER_POSTPROCESS_H_
#define INC_MODULE_RENDER_POSTPROCESS_H_

#include "app_modules/app_module.h"
#include "render/render.h"

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

	//template< typename TObj >
	CHandle GetFX(std::string name);


	void ActivateFXAtEnd(std::string name, int priority = 0);
	void ActivateFXAtEnd(TCompBasicFX* handle, int priority = 0);

	void RemoveActiveFX(std::string name, int priority);

	void RemoveActiveFX(TCompBasicFX * handle, int priority);

	void ActivateFXBeforeUI(std::string name, int priority = 0);
	void ActivateFXBeforeUI(TCompBasicFX* handle, int priority = 0);

	void AddFX(std::string name, TCompBasicFX* handle);

	static bool sortByPriority(const TKeyFX &k1, const TKeyFX &k2);

	void stop();
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

#endif