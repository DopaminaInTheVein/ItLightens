#ifndef INC_MODULE_RENDER_DEFERRED_H_
#define INC_MODULE_RENDER_DEFERRED_H_

#include "app_modules/app_module.h"
#include "render/render.h"

#include "render\render_instanced.h"

class CRenderDeferredModule : public IAppModule {
	enum typeObject {
		POL_MINUS_OBJECTS = 1,
		POL_PLUS_OBJECTS,
		SHOTS_OBJECTS,
		INTERACTIVE_OBJECTS,
		VISION_OBJECTS_RED,
		VISION_OBJECTS_GREEN,
		VISION_OBJECTS_YELLOW,
		VISION_OBJECTS_WHITE
	};

	//final output
	CRenderToTexture* rt_output;

	//basic textures
	CRenderToTexture* rt_albedos;
	CRenderToTexture* rt_normals;
	CRenderToTexture* rt_depths;
	CRenderToTexture* rt_selfIlum;
	CRenderToTexture* rt_acc_light;
	CRenderToTexture* rt_final;
	

	//lights
	CRenderToTexture* rt_specular;
	CRenderToTexture* rt_specular_lights;
	CRenderToTexture* rt_glossiness;

	//shadows
	CRenderToTexture* rt_ssao;
	CRenderToTexture* rt_shadows_gl;

	//aux
	CRenderToTexture* rt_data;
	CRenderToTexture* rt_data2;
	CRenderToTexture* rt_black;

	//temp
	CRenderToTexture* rt_selfIlum_int;
	CRenderToTexture* rt_selfIlum_blurred_int;
	CRenderToTexture* rt_selfIlum_blurred;
	

	int xres, yres;
	bool ssao_test = true;
	int generate_shadow_maps = 0;

	CHandle                 h_camera;
	CHandle                 h_ui_camera;

	const CRenderTechnique* acc_light_points;
	const CRenderTechnique* acc_light_directionals;
	const CRenderTechnique* blur_tech;
	const CRenderTechnique* null_tech;
	const CRenderTechnique* skining_tech;
	const CRenderTechnique* acc_light_directionals_shadows;
	const CMesh*            unit_sphere;
	const CMesh*            unit_cube;
	const CMesh*			particles_mesh;

	//fast fix
	CRenderParticlesInstanced helpers;

	bool m_isSpecialVisionActive = false;
	bool test_dream_shader = false;

	void renderGBuffer();
	void activateRenderCamera3D();
	void renderAccLight();
	void RenderPolarizedPP(int pol, const VEC4& color);
	void ApplySSAO();
	void MarkInteractives(const VEC4 & color, std::string, int slot);
	void ShootGuardRender();
	void addPointLights();
	void addDirectionalLights();
	void addDirectionalLightsShadows();
	void addAmbientPass();
	void generateShadowMaps();
	void generateStaticShadowMaps();	//to generate static shadow maps

	void FinalRender();

	void blurEffectLights(bool intermitent);

public:
	bool start() override;
	void stop() override;
	void update(float dt);
	void render() override;
	void UpdateStaticShadowMaps() {
		generate_shadow_maps = 350; //will be put on false after generating static shadow maps
	}
	void renderEspVisionMode();
	void renderEspVisionModeFor(std::string tag, VEC4 color_mask, int stencil_mask, bool use_skeleton = false, bool only_borders = true);
	void renderDetails(CRenderTechnique::eCategory type);
	void uploadConstantsGPU();
	void applyPostFX();
	void renderUI();
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "render_deferred";
	}

	void SetOutputDeferred();
	CTexture* GetOutputTexture() const {
		return rt_output;
	}

	CTexture* GetFinalTexture() const {
		return rt_final;
	}

	float getXRes() { return xres; }
	float getYRes() { return yres; }
	float getAspectRatio();
};

#endif
