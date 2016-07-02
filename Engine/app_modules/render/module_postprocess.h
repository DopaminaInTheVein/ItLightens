#ifndef INC_MODULE_POSTPROCESS_H_
#define INC_MODULE_POSTPROCESS_H_

#include "app_modules/app_module.h"
#include "app_modules/postprocess.h"

typedef std::vector<IPostProcess*> all_fx;

class CPostProcessModule : public IAppModule {
	all_fx fxs;

public:
	bool start() override;
	void stop() override;
	void update(float dt);
	void render() override;
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "render_deferred";
	}
};

#endif