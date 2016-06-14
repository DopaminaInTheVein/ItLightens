#ifndef INC_MODULE_RENDER_PP_H_
#define INC_MODULE_RENDER_PP_H_

#include "app_modules/app_module.h"
#include "render/render.h"

class CRenderPostProcessModule : public IAppModule {
	std::vector<std::string> order_fx;

public:
	bool start() override;
	void stop() override;
	void update(float dt);
	void render() override;
	const char* getName() const {
		return "render_postprocess";
	}
};

#endif
