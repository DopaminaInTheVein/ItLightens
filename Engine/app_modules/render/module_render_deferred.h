#ifndef INC_MODULE_RENDER_DEFERRED_H_
#define INC_MODULE_RENDER_DEFERRED_H_

#include "app_modules/app_module.h"
#include "render/render.h"

class CRenderDeferredModule : public IAppModule {
	CRenderToTexture* rt_albedos;
	CRenderToTexture* rt_normals;
	CRenderToTexture* rt_wpos;
	CRenderToTexture* rt_acc_light;
	int xres, yres;

	const CRenderTechnique* acc_light_points;
	const CRenderTechnique* acc_light_directionals;
	const CMesh*            unit_sphere;
	const CMesh*            unit_cube;

	void renderGBuffer();
	void renderAccLight();
	void addPointLights();
	void addDirectionalLights();

public:
	bool start() override;
	void stop() override;
	void render() override;
	const char* getName() const {
		return "render_deferred";
	}
};

#endif
