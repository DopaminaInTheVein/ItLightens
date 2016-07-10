#ifndef INC_MODULE_RENDER_DEFERRED_H_
#define INC_MODULE_RENDER_DEFERRED_H_

#include "app_modules/app_module.h"
#include "render/render.h"

#include "render\render_instanced.h"

class CRenderDeferredModule : public IAppModule {
	CRenderToTexture* rt_albedos;
	CRenderToTexture* rt_normals;
	CRenderToTexture* rt_depths;
	CRenderToTexture* rt_selfIlum;
	CRenderToTexture* rt_acc_light;
	CRenderToTexture* rt_selfIlum_blurred;
	CRenderToTexture* rt_final;

	CRenderToTexture* rt_specular;

	CRenderToTexture* rt_shadows;

	CRenderToTexture* rt_data;
	CRenderToTexture* rt_data2;
	CRenderToTexture* rt_black;

	//temp
	CRenderToTexture* rt_selfIlum_int;
	CRenderToTexture* rt_selfIlum_blurred_int;
	CRenderToTexture* rt_temp;

	int xres, yres;

	CHandle                 h_camera;	
	
	const CRenderTechnique* acc_light_points;
	const CRenderTechnique* acc_light_directionals;
	const CRenderTechnique* blur_tech;
	const CRenderTechnique* acc_light_directionals_shadows;
	const CMesh*            unit_sphere;
	const CMesh*            unit_cube;
	const CMesh*			particles_mesh;

	//fast fix
	CRenderParticlesInstanced helpers;

	void renderGBuffer();
	void activateRenderCamera3D();
	void renderAccLight();
	void RenderPolarizedPP(int pol, const VEC4& color);
	void MarkInteractives(const VEC4 & color);
	void ShootGuardRender();
	void addPointLights();
	void addDirectionalLights();
	void addDirectionalLightsShadows();
	void addAmbientPass();
  	void generateShadowMaps();

	void FinalRender();

	void blurEffectLights(bool intermitent);

public:
	bool start() override;
	void stop() override;
	void update(float dt);
	void render() override;
	void renderDetails();
	void applyPostFX();
	void RenderHelpGenLoc();
	void renderUI();
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "render_deferred";
	}
};

#endif
