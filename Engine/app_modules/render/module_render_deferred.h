#ifndef INC_MODULE_RENDER_DEFERRED_H_
#define INC_MODULE_RENDER_DEFERRED_H_

#include "app_modules/app_module.h"
#include "render/render.h"

class CRenderDeferredModule : public IAppModule {
	CRenderToTexture* rt_albedos;
	CRenderToTexture* rt_normals;
	CRenderToTexture* rt_wpos;
	CRenderToTexture* rt_selfIlum;
	CRenderToTexture* rt_acc_light;
	CRenderToTexture* rt_selfIlum_blurred;
	CRenderToTexture* rt_depthTexture;
	CRenderToTexture* rt_final;

	CRenderToTexture* rt_data;
	CRenderToTexture* rt_black;

	int xres, yres;

	const CRenderTechnique* acc_light_points;
	const CRenderTechnique* acc_light_directionals;
	const CRenderTechnique* blur_tech;
	const CMesh*            unit_sphere;
	const CMesh*            unit_cube;

	void renderGBuffer();
	void renderAccLight();
	void DepthTexture();
	void RenderPolarizedPP(int pol, const VEC4& color);
	void GlowEdges();
	void addPointLights();
	void addDirectionalLights();

	void FinalRender();

	void blurEffectLights();

public:
	bool start() override;
	void stop() override;
	void update(float dt);
	void render() override;
	const char* getName() const {
		return "render_deferred";
	}
};

#endif
