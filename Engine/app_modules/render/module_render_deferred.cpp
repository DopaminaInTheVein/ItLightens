#include "mcv_platform.h"
#include "module_render_deferred.h"
#include "camera/camera.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_camera_main.h"
#include "components/comp_light_dir.h"
#include "components/comp_light_dir_shadows.h"
#include "components/comp_light_dir_shadows_dyn.h"
#include "components/comp_light_point.h"
#include "components/comp_light_fadable.h"
#include "skeleton/comp_skeleton.h"
#include "render/render.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "render/draw_utils.h"
#include "components/comp_render_glow.h"
#include "render\static_mesh.h"
#include "app_modules/logic_manager/logic_manager.h"

#include "components\comp_render_fade_screen.h"

#include "render/fx/GuardShots.h"
#include "components\comp_room.h"

#include "module_render_postprocess.h"

//Particles
#include "particles\particles_manager.h"

//POLARIZE
#include "components/comp_polarized.h"
#include "components\comp_transform.h"
#include "components\comp_render_static_mesh.h"
#include "render\static_mesh.h"
#include "components\comp_life.h"
#include "player_controllers\player_controller.h"


#include "render\fx\fx_ssao.h"

//for test
#include "test_module_fx.h"
#include "app_modules\io\io.h"

//cubemaps
#include "lights_data\lights_data.h"

extern CRenderPostProcessModule* render_fx;

// ------------------------------------------------------
bool CRenderDeferredModule::start() {
	//xres = CApp::get().getXRes();
	//yres = CApp::get().getYRes();

	xres = Render.getXRes();
	yres = Render.getYRes();

	rt_output = new CRenderToTexture;

	rt_albedos = new CRenderToTexture;
	rt_normals = new CRenderToTexture;
	rt_depths = new CRenderToTexture;
	rt_acc_light = new CRenderToTexture;
	rt_selfIlum = new CRenderToTexture;
	rt_selfIlum_blurred = new CRenderToTexture;
	rt_final = new CRenderToTexture;

	rt_specular = new CRenderToTexture;
	rt_specular_lights = new CRenderToTexture;
	rt_glossiness = new CRenderToTexture;
	rt_shadows_gl = new CRenderToTexture;

	//aux
	rt_data = new CRenderToTexture;
	rt_data2 = new CRenderToTexture;
	rt_black = new CRenderToTexture;
	rt_ssao = new CRenderToTexture;

	//temp
	rt_selfIlum_int = new CRenderToTexture;
	rt_selfIlum_blurred_int = new CRenderToTexture;

	if (!rt_output->createRT("rt_output", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;

	if (!rt_specular->createRT("rt_specular", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_glossiness->createRT("rt_glossiness", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_specular_lights->createRT("rt_specular_lights", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_albedos->createRT("rt_albedo", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_data->createRT("rt_data", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_data2->createRT("rt_data2", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_black->createRT("rt_black", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_normals->createRT("rt_normals", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_depths->createRT("rt_depths", xres, yres, DXGI_FORMAT_R16_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_selfIlum->createRT("rt_selfIlum", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_acc_light->createRT("rt_acc_light", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_selfIlum_blurred->createRT("rt_selfIlum_blurred", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_final->createRT("rt_final", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;

	//temp
	if (!rt_selfIlum_int->createRT("rt_selfIlum_int", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_selfIlum_blurred_int->createRT("rt_selfIlum_blurred_int", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_ssao->createRT("rt_ssao", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_shadows_gl->createRT("rt_shadows_gl", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;

	//
	acc_light_points = Resources.get("deferred_lights_point.tech")->as<CRenderTechnique>();
	assert(acc_light_points && acc_light_points->isValid());

	acc_light_directionals = Resources.get("deferred_lights_dir.tech")->as<CRenderTechnique>();
	assert(acc_light_directionals && acc_light_directionals->isValid());

	//blur_tech = Resources.get("blur_glow.tech")->as<CRenderTechnique>();

	acc_light_directionals_shadows = Resources.get("deferred_lights_dir_shadows.tech")->as<CRenderTechnique>();

	//null technique
	null_tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
	//skinning technique
	skining_tech = Resources.get("shadow_gen_skin.tech")->as<CRenderTechnique>();

	assert(acc_light_directionals_shadows && acc_light_directionals_shadows->isValid());

	//  unit_sphere = Resources.get("meshes/engine/unit_sphere.mesh")->as<CMesh>();
	unit_sphere = Resources.get("unitQuadXY.mesh")->as<CMesh>();
	assert(unit_sphere && unit_sphere->isValid());
	unit_cube = Resources.get("meshes/engine/unit_frustum.mesh")->as<CMesh>();
	assert(unit_cube && unit_cube->isValid());

	particles_mesh = Resources.get("textured_quad_xy_centered.mesh")->as<CMesh>();

	Resources.get("textures/general/noise.dds")->as<CTexture>()->activate(TEXTURE_SLOT_NOISE);

	Resources.get("textures/ramps/rampa_prueba.dds")->as<CTexture>()->activate(TEXTURE_SLOT_RAMP);

	//upload cubemaps
	TContainerCubemaps c;
	c.uploadTextures();

	shader_ctes_globals.use_ramp = 1;
	shader_ctes_globals.world_time = 0.f;
	shader_ctes_globals.xres = xres;
	shader_ctes_globals.yres = yres;
	shader_ctes_globals.env_factor = 0.170f;
	shader_ctes_globals.shadow_intensity = 0.5f;

	return true;
}

// ------------------------------------------------------
void CRenderDeferredModule::stop() {
	//RenderManager.clear();
}

static bool test_reflections = false;
// ------------------------------------------------------
void CRenderDeferredModule::update(float dt) {
	shader_ctes_globals.world_time += dt;

	if (controller->isTestSSAOButoonPressed()) {
		ssao_test = !ssao_test;
	}
	if (io->keys[VK_F1].becomesPressed()) {
		test_dream_shader = !test_dream_shader;
	}

	if (io->keys[VK_F6].becomesPressed()) {
		UpdateStaticShadowMaps();
	}
#ifndef NDEBUG
	if (io->keys[VK_F5].becomesPressed()) {
		test_reflections = !test_reflections;
	}
#endif

	//m_isSpecialVisionActive = tags_manager.getFirstHavingTag(getID("player")).hasTag("raijin") && controller->IsSenseButtonPressed();
	m_isSpecialVisionActive = GameController->isSenseVisionEnabled();
	//if (controller->SenseButtonBecomesPressed()) {
	//	std::string level = CApp::get().getCurrentRealLevel() + "_sense_pressed()";
	//	logic_manager->throwUserEvent(level, "");
	//}
}

// ------------------------------------------------------
void CRenderDeferredModule::renderGBuffer() {
	PROFILE_FUNCTION("GBuffer");
	CTraceScoped scope("GBuffer");
	//leidas al principio del render
	///h_camera = tags_manager.getFirstHavingTag(getID("camera_main"));
	///h_ui_camera = tags_manager.getFirstHavingTag(getID("ui_camera"));

	//static CCamera camera;

	//if (h_camera.isValid()) {
	//  CEntity* e = h_camera;
	//  TCompCamera* comp_cam = e->get<TCompCamera>();
	//  camera = *comp_cam;
	//  camera.setAspectRatio((float)xres / (float)yres);
	//  comp_cam->setAspectRatio(camera.getAspectRatio());
	//}

	// To set a default and known Render State
	Render.ctx->RSSetState(nullptr);
	activateZ(ZCFG_DEFAULT);
	activateBlend(BLENDCFG_DEFAULT);

	// Activo la camara en la pipeline de render
	//activateCamera(&camera);
	activateRenderCamera3D();

	// -------------------------
	// Activar mis multiples render targets
	ID3D11RenderTargetView* rts[6] = {
	  rt_albedos->getRenderTargetView()
	  ,	rt_normals->getRenderTargetView()
	  ,	rt_depths->getRenderTargetView()
	  ,   rt_selfIlum->getRenderTargetView()
	  , rt_specular->getRenderTargetView()
	  , rt_glossiness->getRenderTargetView()
	};
	// Y el ZBuffer del backbuffer principal
	Render.ctx->OMSetRenderTargets(6, rts, Render.depth_stencil_view);
	rt_albedos->activateViewport();
	// Clear de los render targets y el ZBuffer
	rt_albedos->clear(VEC4(0.05, 0.0588, 0.169, 1));
	rt_normals->clear(VEC4(0, 1, 0, 1));
	rt_selfIlum->clear(VEC4(0, 0, 0, 1));
	rt_depths->clear(VEC4(1, 1, 1, 1));
	rt_final->clear(VEC4(0, 0, 0, 0));
	rt_specular_lights->clear(VEC4(0, 0, 0, 0));

	Render.clearMainZBuffer();

	rt_acc_light->clear(VEC4(0, 0, 0, 1));
	rt_shadows_gl->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_blurred->clear(VEC4(0, 0, 0, 1));

	// Activa la ctes del object
	activateWorldMatrix(MAT44::Identity);

	// Mandar a pintar los 'solidos'
	if (h_camera.isValid())
		RenderManager.renderAll(h_camera, CRenderTechnique::SOLID_OBJS, SBB::readSala());

	activateZ(ZCFG_DEFAULT);
}

// ----------------------------------------------
void CRenderDeferredModule::activateRenderCamera3D() {
	if (!h_camera.isValid()) return;
	GET_COMP(comp_cam, h_camera, TCompCameraMain);
	comp_cam->setAspectRatio((float)xres / (float)yres);

	// Copy the render aspect ratio back to the comp_camera
	// of the entity, so the culling uses the real view_proj
	comp_cam->setAspectRatio(comp_cam->getAspectRatio());

	// Activo la camara en la pipeline de render
	activateCamera(comp_cam);

	// Activa la ctes del object
	activateWorldMatrix(MAT44::Identity);
}

// ----------------------------------------------
void CRenderDeferredModule::addPointLights() {
	PROFILE_FUNCTION("addPointLights");
	CTraceScoped scope("addPointLights");

	// Activar la tech deferred_lights_point.tech
	acc_light_points->activate();

	// Activar la mesh solo UNA vez
	const CMesh* mesh = unit_sphere;
	mesh->activate();

	// Activar la mesh unit_sphere
	getHandleManager<TCompLightPoint>()->each([mesh](TCompLightPoint* c) {
		PROFILE_FUNCTION("upload point light");
		GET_COMP(room, CHandle(c).getOwner(), TCompRoom);
		if (room && !room->sameRoomPlayer()) return;
		// Subir todo lo que necesite la luz para pintarse en el acc light buffer
		// la world para la mesh y las constantes en el pixel shader
		c->activate();
		// Pintar la mesh que hemos activado hace un momento
		mesh->render();
	});
}

// ----------------------------------------------
void CRenderDeferredModule::addDirectionalLights() {
	PROFILE_FUNCTION("addDirectionalLights");
	CTraceScoped scope("addDirectionalLights");

	// Activar la tech acc_light_directionals.tech
	acc_light_directionals->activate();

	// Activar la mesh solo UNA vez
	const CMesh* mesh = unit_cube;
	mesh->activate();

	// Activar la mesh unit_sphere
	getHandleManager<TCompLightDir>()->each([mesh](TCompLightDir* c) {
		// Subir todo lo que necesite la luz para pintarse en el acc light buffer
		// la world para la mesh y las constantes en el pixel shader
		PROFILE_FUNCTION("upload light dir");
		GET_COMP(room, CHandle(c).getOwner(), TCompRoom);
		if (room && !room->sameRoomPlayer()) return;

		c->activate();
		// Pintar la mesh que hemos activado hace un momento
		mesh->render();
	});
}

// ----------------------------------------------
void CRenderDeferredModule::addDirectionalLightsShadows() {
	PROFILE_FUNCTION("addDirectionalLightsShadows");
	CTraceScoped scope("addDirectionalLightsShadows");

	// Activar la tech acc_light_directionals_shadows.tech
	acc_light_directionals_shadows->activate();

	// Activar la mesh solo UNA vez
	const CMesh* mesh = unit_cube;
	mesh->activate();

	// Activar la mesh unit_sphere
	getHandleManager<TCompLightDirShadows>()->each([mesh](TCompLightDirShadows* c) {
		// Subir todo lo que necesite la luz para pintarse en el acc light buffer
		// la world para la mesh y las constantes en el pixel shader
		PROFILE_FUNCTION("upload shadow dir");

		c->activate();
		// Pintar la mesh que hemos activado hace un momento
		mesh->render();
	});

	getHandleManager<TCompLightDirShadowsDynamic>()->each([mesh](TCompLightDirShadowsDynamic* c) {
		// Subir todo lo que necesite la luz para pintarse en el acc light buffer
		// la world para la mesh y las constantes en el pixel shader
		PROFILE_FUNCTION("upload shadow dir");

		if (c->isInRoom()) {
			c->activate();
			// Pintar la mesh que hemos activado hace un momento
			mesh->render();
		}
	});

	CTexture::deactivate(TEXTURE_SLOT_SHADOWMAP);
	CTexture::deactivate(TEXTURE_SLOT_SHADOWMAP_STATICS);
}

void CRenderDeferredModule::addAmbientPass() {
	PROFILE_FUNCTION("addAmbientPass");
	CTraceScoped scope("addAmbientPass");
	activateZ(ZCFG_ALL_DISABLED);

	auto tech = Resources.get("deferred_add_ambient.tech")->as<CRenderTechnique>();
	tech->activate();

	drawFullScreen(rt_albedos, tech);
}

void CRenderDeferredModule::FinalRender() {
	PROFILE_FUNCTION("final_texture");
	CTraceScoped scope("final_texture");

	ID3D11RenderTargetView* rts[3] = {
	  rt_final->getRenderTargetView()
	  ,	nullptr   // remove the other rt's from the pipeline
	  ,	nullptr
	};

	//Render.activateBackBuffer();	//reset size to default
	SetOutputDeferred();

	// Y el ZBuffer del backbuffer principal
	Render.ctx->OMSetRenderTargets(3, rts, nullptr);

	/*	rt_albedos->activate(TEXTURE_SLOT_DIFFUSE);
		rt_acc_light->activate(TEXTURE_SLOT_ENVIRONMENT);
		rt_selfIlum->activate(TEXTURE_SLOT_SELFILUM);
		rt_depths->activate(TEXTURE_SLOT_DEPTHS);
		rt_normals->activate(TEXTURE_SLOT_NORMALS);*/

	activateZ(ZCFG_ALL_DISABLED);
	activateBlend(BLENDCFG_DEFAULT);
	//auto tech = Resources.get("deferred_add_ambient.tech")->as<CRenderTechnique>();
	drawFullScreen(rt_acc_light);

	activateZ(ZCFG_DEFAULT);
}

void CRenderDeferredModule::blurEffectLights(bool intermitent) {
	PROFILE_FUNCTION("blurEffectLights");
	CTraceScoped scope("blurEffectLights");

	CRenderToTexture *normal;
	CRenderToTexture *blurred;
	const CRenderTechnique* tech_blur;

	if (!intermitent) {
		normal = rt_selfIlum;
		blurred = rt_selfIlum_blurred;
		tech_blur = blur_tech;
	}
	else {
		normal = rt_selfIlum_int;
		blurred = rt_selfIlum_blurred_int;
		tech_blur = Resources.get("blur_int.tech")->as<CRenderTechnique>();
	}

	blurred->clear(VEC4(0, 0, 0, 0));

	ID3D11RenderTargetView* rts[3] = {
	  blurred->getRenderTargetView()
	  ,	nullptr   // remove the other rt's from the pipeline
	  ,	nullptr
	};
	// Y el ZBuffer del backbuffer principal
	Render.ctx->OMSetRenderTargets(3, rts, nullptr);

	activateZ(ZCFG_ALL_DISABLED);

	//auto tech = Resources.get("deferred_add_ambient.tech")->as<CRenderTechnique>();
	drawFullScreen(normal, tech_blur);

	activateZ(ZCFG_DEFAULT);
	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
}

#include "render\fx\fx_glow.h"
// ----------------------------------------------
void CRenderDeferredModule::renderAccLight() {
	PROFILE_FUNCTION("renderAccLight");
	CTraceScoped scope("renderAccLight");

	// Activar el rt para pintar las luces...
	ID3D11RenderTargetView* rts[3] = {
	  rt_acc_light->getRenderTargetView()
	  ,  rt_specular_lights->getRenderTargetView()
	  ,  rt_shadows_gl->getRenderTargetView()
	};
	// Y el ZBuffer del backbuffer principal
	Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

	// Activar las texturas del gbuffer en la pipeline para
	// que se puedan acceder desde los siguientes shaders
	//rt_albedos->activate(TEXTURE_SLOT_DIFFUSE);	//activated on addAmbientPass
	rt_depths->activate(TEXTURE_SLOT_DEPTHS);
	rt_normals->activate(TEXTURE_SLOT_NORMALS);
	rt_specular->activate(TEXTURE_SLOT_SPECULAR_GL);
	rt_glossiness->activate(TEXTURE_SLOT_GLOSSINESS);

	//rt_acc_light->clear(VEC4(0, 0, 0, 1));

	activateBlend(BLENDCFG_DEFAULT);
	addAmbientPass();

	activateBlend(BLENDCFG_ADDITIVE);
	activateZ(ZCFG_LIGHTS_CONFIG);
	//activateRS(RSCFG_INVERT_CULLING);
	addPointLights();

	activateZ(ZCFG_LIGHTS_DIR_CONFIG);
	//activateRS(RSCFG_INVERT_CULLING);
	addDirectionalLights();

	activateBlend(BLENDCFG_ADDITIVE);
	activateRS(RSCFG_DEFAULT);
	addDirectionalLightsShadows();

	CTexture* blurred_shadows = rt_shadows_gl;

	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);

	activateBlend(BLENDCFG_DEFAULT);
	activateZ(ZCFG_DEFAULT);

	//blur shadows
	TRenderGlow* glow = render_fx->GetFX<TRenderGlow>("blur");
	if (glow)
		blurred_shadows = glow->apply(blurred_shadows);

	//reset size
	//Render.activateBackBuffer();
	SetOutputDeferred();


	{
		// Activar el rt para pintar las luces...
		ID3D11RenderTargetView* rts[3] = {
			rt_acc_light->getRenderTargetView()
			,  nullptr
			,  nullptr
		};
		// Y el ZBuffer del backbuffer principal
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		activateZ(ZCFG_ALL_DISABLED);
		//activateBlend(BLENDCFG_COMBINATIVE);
		activateBlend(BLENDCFG_SUBSTRACT);
		drawFullScreen(blurred_shadows);
		//CTexture::deactivate(TEXTURE_SLOT_SHADOWS);
	}

	{
		rt_specular_lights->activate(TEXTURE_SLOT_SPECULAR_GL);
		//ss_reflections
		ID3D11RenderTargetView* rts[3] = {
			rt_acc_light->getRenderTargetView()
			,  nullptr
			,  nullptr
		};
		// Y el ZBuffer del backbuffer principal
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		activateBlend(BLENDCFG_ADDITIVE);
		if(test_reflections)activateBlend(BLENDCFG_DEFAULT);
		activateZ(ZCFG_ALL_DISABLED);

		auto tech = Resources.get("ss_reflections.tech")->as<CRenderTechnique>();

		drawFullScreen(rt_albedos, tech);
	}

	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	activateRS(RSCFG_DEFAULT);
	activateZ(ZCFG_DEFAULT);
	activateBlend(BLENDCFG_DEFAULT);

	/*CTexture::deactivate(TEXTURE_SLOT_SPECULAR_GL);
	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	CTexture::deactivate(TEXTURE_SLOT_NORMALS);
	CTexture::deactivate(TEXTURE_SLOT_DEPTHS);*/
}

// ----------------------------------------------
void CRenderDeferredModule::generateStaticShadowMaps() {
	PROFILE_FUNCTION("generateShadowMaps");
	CTraceScoped scope("generateShadowMaps");

	// Llamar al metodo generateShadowMap para todas los components de tipo dir_shadows
	//getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::generateShadowMap);

	getHandleManager<TCompLightDirShadows>()->each([](TCompLightDirShadows* c) {
		// Subir todo lo que necesite la luz para pintarse en el acc light buffer
		// la world para la mesh y las constantes en el pixel shader
		PROFILE_FUNCTION("check gen shadow");

		c->generateStaticShadowMap();
	});

	--generate_shadow_maps ;
}

// ----------------------------------------------
void CRenderDeferredModule::generateShadowMaps() {
	PROFILE_FUNCTION("generateShadowMaps");
	CTraceScoped scope("generateShadowMaps");

	// Llamar al metodo generateShadowMap para todas los components de tipo dir_shadows
	//getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::generateShadowMap);

	getHandleManager<TCompLightDirShadows>()->each([](TCompLightDirShadows* c) {
		// Subir todo lo que necesite la luz para pintarse en el acc light buffer
		// la world para la mesh y las constantes en el pixel shader
		PROFILE_FUNCTION("check gen shadow");

		c->generateShadowMap();
	});

	getHandleManager<TCompLightDirShadowsDynamic>()->each([](TCompLightDirShadowsDynamic* c) {
		// Subir todo lo que necesite la luz para pintarse en el acc light buffer
		// la world para la mesh y las constantes en el pixel shader
		PROFILE_FUNCTION("check gen shadow");

		if(c->isInRoom()) 
			c->generateShadowMap();
	});
}

void CRenderDeferredModule::SetOutputDeferred()
{
	ID3D11RenderTargetView* rts[3] = {
		rt_output->getRenderTargetView()
		,	nullptr   // remove the other rt's from the pipeline
		,	nullptr
	};
	Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
	rt_output->activateViewport();
}

void CRenderDeferredModule::RenderPolarizedPP(int pol, const VEC4& color) {
	shader_ctes_globals.global_color = color;
	shader_ctes_globals.uploadToGPU();

	activateBlend(BLENDCFG_COMBINATIVE);

	//create mask
	{
		PROFILE_FUNCTION("referred: mask");
		CTraceScoped scope("mask");

		//activateZ(ZCFG_DEFAULT);
		activateZ(ZCFG_MASK_NUMBER, pol);

		ID3D11RenderTargetView* rts[3] = {
		  rt_data->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		//auto tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
		null_tech->activate();

		getHandleManager<TCompPolarized>()->each([pol](TCompPolarized* c) {
			if (c->getForce().polarity == pol) {	//render polarity designed only
				CHandle h = CHandle(c).getOwner();
				GET_COMP(rsm, h, TCompRenderStaticMesh);
				GET_COMP(c_tmx, h, TCompTransform);
				if (c_tmx) activateWorldMatrix(c_tmx->asMatrix());
				if (rsm) rsm->static_mesh->slots[0].mesh->activateAndRender();
			}
		});
	}

	//edge detection
	{
		PROFILE_FUNCTION("referred: edge detection");
		CTraceScoped scope("edge detection final");

		// Activar el rt para pintar las luces...

		ID3D11RenderTargetView* rts[3] = {
		  rt_selfIlum->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		// Y el ZBuffer del backbuffer principal

		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		rt_depths->activate(TEXTURE_SLOT_DEPTHS);
		rt_normals->activate(TEXTURE_SLOT_NORMALS);

		activateZ(ZCFG_OUTLINE, pol);
		//activateZ(ZCFG_ALL_DISABLED);

		auto tech = Resources.get("edgeDetection.tech")->as<CRenderTechnique>();

		drawFullScreen(rt_black, tech);
		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}
}

void CRenderDeferredModule::ApplySSAO() {
	//Render.activateBackBuffer();
	PROFILE_FUNCTION("referred: ssao");
	CTraceScoped scope("referred: ssao");

	rt_ssao->clear(VEC4(0, 0, 0, 0));

	rt_normals->activate(TEXTURE_SLOT_NORMALS);
	rt_depths->activate(TEXTURE_SLOT_DEPTHS);

/*
	ID3D11RenderTargetView* rts[3] = {
		//Render.render_target_view
		rt_ssao->getRenderTargetView()
		,   nullptr
		,	nullptr
	};

	Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
	activateBlend(BLENDCFG_DEFAULT);

	activateZ(ZCFG_ALL_DISABLED);

	rt_normals->activate(TEXTURE_SLOT_NORMALS);
	rt_depths->activate(TEXTURE_SLOT_DEPTHS);
	auto tech = Resources.get("ssao.tech")->as<CRenderTechnique>();

	drawFullScreen(rt_albedos, tech);

	//Render.activateBackBuffer();
	SetOutputDeferred();
	//blur shadows
	CTexture *blurred_ssao = rt_ssao;
	GET_COMP(glow, h_camera, TCompRenderGlow);
	//glow->
	if (glow)
		blurred_ssao = glow->apply(blurred_ssao);

	//Render.activateBackBuffer();	//reset size viewport
	SetOutputDeferred();
	activateZ(ZCFG_ALL_DISABLED);

	{
		ID3D11RenderTargetView* rts[3] = {
			rt_acc_light->getRenderTargetView()
			, rt_shadows_gl->getRenderTargetView()
			,   nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		auto tech = Resources.get("solid_textured_multiple_outputs.tech")->as<CRenderTechnique>();

		activateBlend(BLENDCFG_SUBSTRACT);
		//activateBlend(BLENDCFG_DEFAULT);	//for testing only
		drawFullScreen(blurred_ssao, tech);
	}
	activateBlend(BLENDCFG_DEFAULT);
	//Render.activateBackBuffer();
	SetOutputDeferred();*/

	/*CTexture::deactivate(TEXTURE_SLOT_DEPTHS);
	CTexture::deactivate(TEXTURE_SLOT_NORMALS);*/
	
	TRenderSSAO* ssao_fx = render_fx->GetFX<TRenderSSAO>("ssao");
	ssao_fx->GetOcclusionTextue(h_camera, rt_ssao, rt_acc_light, rt_shadows_gl);
}

void CRenderDeferredModule::MarkInteractives(const VEC4& color, std::string tag, int slot) {
	shader_ctes_globals.global_color = color;
	shader_ctes_globals.uploadToGPU();

	//create mask
	{
		PROFILE_FUNCTION("referred: mask");
		CTraceScoped scope("mask");

		//activateZ(ZCFG_DEFAULT);
		activateZ(ZCFG_MASK_NUMBER, slot);

		ID3D11RenderTargetView* rts[3] = {
			rt_data->getRenderTargetView()
			,	nullptr   // remove the other rt's from the pipeline
			,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		//auto tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
		null_tech->activate();

		auto hs = tags_manager.getHandlesByTag(tag);

		for (CEntity* e : hs) {
			if (!e) continue;
			TCompRenderStaticMesh *rsm = e->get<TCompRenderStaticMesh>();
			TCompTransform *c_tmx = e->get<TCompTransform>();
			if (!c_tmx || !rsm) continue;
			activateWorldMatrix(c_tmx->asMatrix());

			//rsm->static_mesh->slots[0].material->activateTextures();
			rsm->static_mesh->slots[0].mesh->activateAndRender();

			//rsm->static_mesh->slots[0].material->deactivateTextures();
		}
	}

	activateZ(ZCFG_OUTLINE, slot);
	{
		activateBlend(BLENDCFG_ADDITIVE);
		//Render.activateBackBuffer();
		SetOutputDeferred();
		auto tech = Resources.get("LightenInteractive.tech")->as<CRenderTechnique>();
		drawFullScreen(rt_final, tech);
		activateBlend(BLENDCFG_COMBINATIVE);
	}

	//edge detection
	{
		PROFILE_FUNCTION("referred: edge detection");
		CTraceScoped scope("edge detection final");

		// Activar el rt para pintar las luces...

		ID3D11RenderTargetView* rts[3] = {
			rt_selfIlum->getRenderTargetView()
			,	nullptr   // remove the other rt's from the pipeline
			,	nullptr
		};
		// Y el ZBuffer del backbuffer principal

		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		rt_depths->activate(TEXTURE_SLOT_DEPTHS);
		rt_normals->activate(TEXTURE_SLOT_NORMALS);

		//activateZ(ZCFG_ALL_DISABLED);

		auto tech = Resources.get("edgeDetection.tech")->as<CRenderTechnique>();

		drawFullScreen(rt_black, tech);
		//rt_black->clear(VEC4(0, 0, 0, 1)); //we dont care about that texture, clean black texture
		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}
}

void CRenderDeferredModule::ShootGuardRender() {
	// Fx
	{
		PROFILE_FUNCTION("referred: mask laser");
		CTraceScoped scope("mask laser");
		rt_black->clear(VEC4(0, 0, 0, 0));
		//activateZ(ZCFG_DEFAULT);
		activateZ(ZCFG_MASK_NUMBER, SHOTS_OBJECTS);
		activateBlend(BLENDCFG_ADDITIVE);
		ID3D11RenderTargetView* rts[3] = {
			rt_black->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		//auto tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
		auto tech = Resources.get("solid_textured.tech")->as<CRenderTechnique>();
		tech->activate();

		//Render.activateBackBuffer();
		//activateZ(ZCFG_DEFAULT);
		ShootManager::renderAll();
	}

	{
		PROFILE_FUNCTION("referred: laser");
		CTraceScoped scope("laser");

		ID3D11RenderTargetView* rts[3] = {
			rt_black->getRenderTargetView()
		,	nullptr   // remove the other rt's from the pipeline
		,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
		//activateBlend(BLENDCFG_ADDITIVE);

		activateZ(ZCFG_OUTLINE, 3);

		auto tech = Resources.get("test_shoot_w.tech")->as<CRenderTechnique>();
		tech->activate();

		rt_black->clear(VEC4(0, 0, 0, 1));
		rt_data2->clear(VEC4(0, 0, 0, 1));
		drawFullScreen(rt_data2, tech);
		//activateZ(ZCFG_DEFAULT);
		//ShootManager::renderAll();
		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}

	{
		PROFILE_FUNCTION("referred: add laser");
		CTraceScoped scope("add laser");
		//Render.activateBackBuffer();
		SetOutputDeferred();
		rt_data2->clear(VEC4(0, 0, 0, 0));
		ID3D11RenderTargetView* rts[3] = {
		  rt_selfIlum->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, nullptr);
		//ractivateBlend(BLENDCFG_ADDITIVE);

		activateZ(ZCFG_ALL_DISABLED);
		activateBlend(BLENDCFG_COMBINATIVE);
		auto tech = Resources.get("test_shoot.tech")->as<CRenderTechnique>();
		tech->activate();

		drawFullScreen(rt_black, tech);

		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}

	if (GameController->GetGameState() == CGameController::RUNNING)ShootManager::shots.clear();
}

// ----------------------------------------------
void CRenderDeferredModule::render() {
	if (GameController->GetGameState() == CGameController::PLAY_VIDEO) return;
	if (!h_camera.isValid()) h_camera = tags_manager.getFirstHavingTag("camera_main");
	if (!h_camera.isValid()) return;
	if (!h_ui_camera.isValid()) h_ui_camera = tags_manager.getFirstHavingTag("ui_camera");
	if (!h_ui_camera.isValid()) return;

	//Render.clearMainZBuffer();
	rt_black->clear(VEC4(0, 0, 0, 1));
	rt_data->clear(VEC4(0, 0, 0, 0));
	rt_specular->clear(VEC4(0, 0, 0, 0));
	rt_glossiness->clear(VEC4(0, 0, 0, 0));

	if(generate_shadow_maps > 0) generateStaticShadowMaps();
	generateShadowMaps();

	rt_data2->clear(VEC4(0, 0, 0, 0));;

	rt_selfIlum->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_int->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_blurred->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_blurred_int->clear(VEC4(0, 0, 0, 0));
	rt_shadows_gl->clear(VEC4(0, 0, 0, 0));

	uploadConstantsGPU();

	renderGBuffer();
	renderDetails(CRenderTechnique::DETAIL_OBJS);
	renderDetails(CRenderTechnique::TRANSPARENT_OBJS);
	renderAccLight();

	if (ssao_test) {
		ApplySSAO();
	}

	//make a texture copy

	//CTexture* copy_blurred_shadows = rt_shadows;

	CEntity* e_camera = h_camera;
	if (!e_camera)
		return;

	//*blurred_shadows = *copy_blurred_shadows;

	//blurEffectLights();

	 //render_particles_instanced.render();

	//MarkInteractives(VEC4(1, 1, 0, 1));

	FinalRender();

	rt_depths->activate(TEXTURE_SLOT_DEPTHS);
	//Render.activateBackBuffer();
	SetOutputDeferred();
	activateZ(ZCFG_ALL_DISABLED);

	drawFullScreen(rt_final);

	g_particlesManager->renderParticles();   //render all particles systems

	activateBlend(BLENDCFG_COMBINATIVE);
	rt_specular_lights->activate(TEXTURE_SLOT_SPECULAR_GL);

	rt_normals->activate(TEXTURE_SLOT_NORMALS);
	//rt_shadows->activate(TEXTURE_SLOT_SHADOWS);

	//activateBlend(BLENDCFG_DEFAULT);	//testing

	//CTexture::deactivate(TEXTURE_SLOT_SHADOWS);

	rt_depths->activate(TEXTURE_SLOT_DEPTHS);

	activateBlend(BLENDCFG_DEFAULT);

	if (GameController->GetFxPolarize()) {
		RenderPolarizedPP(MINUS, VEC4(1.0f, 0.3f, 0.3f, 1.0f));
		RenderPolarizedPP(PLUS, VEC4(0.3f, 0.3f, 1.0f, 1.0f));
	}

	activateZ(ZCFG_DEFAULT);

	ShootGuardRender();

	//Render.activateBackBuffer();
	SetOutputDeferred();
	activateZ(ZCFG_DEFAULT);

	if (m_isSpecialVisionActive) {
		renderEspVisionMode();
	}

	applyPostFX();

	activateZ(ZCFG_ALL_DISABLED);

	rt_shadows_gl->activate(TEXTURE_SLOT_SHADOWS);
	render_fx->ExecuteUILayerFX();

	renderUI();

	activateZ(ZCFG_ALL_DISABLED);
	render_fx->ExecuteAllPendentFX();

	// Leave the 3D Camera active
	activateRenderCamera3D();

	CTexture::deactivate(TEXTURE_SLOT_SHADOWS);
	CTexture::deactivate(TEXTURE_SLOT_SPECULAR_GL);
	CTexture::deactivate(TEXTURE_SLOT_GLOSSINESS);
	CTexture::deactivate(TEXTURE_SLOT_DEPTHS);
	CTexture::deactivate(TEXTURE_SLOT_NORMALS);
	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	CTexture::deactivate(TEXTURE_SLOT_DATA1);
	CTexture::deactivate(TEXTURE_SLOT_DATA2);
	//delete blurred_shadows;
}

void CRenderDeferredModule::renderEspVisionMode() {
	//shader_ctes_globals.global_color = color;
	//shader_ctes_globals.uploadToGPU();

	//darken output
	{
		PROFILE_FUNCTION("referred: darken");
		CTraceScoped scope("darken");

		//Render.activateBackBuffer();
		SetOutputDeferred();
		//rt_depths->activate(TEXTURE_SLOT_DEPTHS);
		activateBlend(BLENDCFG_SUBSTRACT);
		activateZ(ZCFG_ALL_DISABLED);
		//auto tech = Resources.get("darken_screen.tech")->as<CRenderTechnique>();
		//test blur:
		auto tech = Resources.get("especial_vision.tech")->as<CRenderTechnique>();

		activateBlend(BLENDCFG_DEFAULT);
		activateZ(ZCFG_ALL_DISABLED);

		CTexture *tex_screen = rt_final;
		CEntity* e_camera = h_camera;
		TCompRenderGlow* glow = e_camera->get< TCompRenderGlow >();
		if (glow)
			tex_screen = glow->apply(tex_screen, tech);

		//Render.activateBackBuffer();
		SetOutputDeferred();
		activateBlend(BLENDCFG_DEFAULT);
		activateZ(ZCFG_ALL_DISABLED);
		drawFullScreen(tex_screen);
	}

	//MarkInteractives(VEC4(1,1,1,1), "AI", VISION_OBJECTS);
	renderEspVisionModeFor("sense_generator", VEC4(1, 1, 1, 1), VISION_OBJECTS_WHITE);
	renderEspVisionModeFor("tasklist", VEC4(0, 1, 0, 1), VISION_OBJECTS_GREEN, false, false);
	renderEspVisionModeFor("tasklistend", VEC4(1, 1, 0, 1), VISION_OBJECTS_YELLOW, false, false);
	renderEspVisionModeFor("AI_guard", VEC4(1, 0, 0, 1), VISION_OBJECTS_RED, true);
}

void CRenderDeferredModule::renderEspVisionModeFor(std::string tagstr, VEC4 color_mask, int sencil_mask, bool use_skeleton, bool only_borders) {
	//color
	shader_ctes_globals.global_color = color_mask;
	shader_ctes_globals.uploadToGPU();

	//create mask
	{
		//PROFILE_FUNCTION(("referred: mask vision " + tagstr).c_str());
		//CTraceScoped scope(("mask" + tagstr).c_str());
		PROFILE_FUNCTION("referred: mask vision ");
		CTraceScoped scope("mask");

		//activateZ(ZCFG_DEFAULT);
		activateZ(ZCFG_MASK_NUMBER_NO_Z, sencil_mask);

		ID3D11RenderTargetView* rts[3] = {
			rt_data->getRenderTargetView()
			,	nullptr   // remove the other rt's from the pipeline
			,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		if (use_skeleton) {
			skining_tech->activate();
		}
		else {
			//tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
			null_tech->activate();
		}
		// GENERATORS
		VHandles hs = tags_manager.getHandlesByTag(tagstr);
		float outlineWith = 0.1f;
		float offset = 1 + outlineWith;

		//Resources.get("shadow_gen_skin.tech")->as<CRenderTechnique>()->activate();

		bool mesh_uploaded = false;

		for (CHandle h : hs) {
			CEntity* e = h;
			CEntity* e_cam;
			TCompCameraMain *cam;
			TCompRenderStaticMesh *rsm;
			TCompTransform *c_tmx;

			if (!e) continue;
			if (!isInRoom(h)) continue;
			rsm = e->get<TCompRenderStaticMesh>();
			c_tmx = e->get<TCompTransform>();
			if (!c_tmx || !rsm) continue;

			//camera distance
			e_cam = h_camera;
			cam = e_cam->get<TCompCameraMain>();

			float z = fabs(VEC3::Distance(cam->getPosition(), c_tmx->getPosition()));
			{
				PROFILE_FUNCTION("referred: mask vision. Mesh Scale ");
				//set scale bigger for outline
				VEC3 scale = c_tmx->getScale();
				c_tmx->setScale(VEC3(offset, offset, offset)*scale + VEC3(outlineWith, outlineWith, outlineWith)*z*0.1f);

				//push scaled matrix
				activateWorldMatrix(c_tmx->asMatrix());

				//set original scale
				c_tmx->setScale(scale);
			}
			//rsm->static_mesh->slots[0].material->activateTextures();

			//every object will have the same mesh
			if (!mesh_uploaded) {
				PROFILE_FUNCTION("referred: mask vision. Mesh upload ");
				rsm->static_mesh->slots[0].mesh->activate();
				mesh_uploaded = true;
			}

			if (use_skeleton) {
				PROFILE_FUNCTION("referred: mask vision. Skeleton upload ");
				const TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
				assert(comp_skel);
				comp_skel->uploadBonesToCteShader();
			}

			{
				PROFILE_FUNCTION("referred: mask vision. Render ");
				//rsm->static_mesh->slots[0].mesh->render();
				rsm->static_mesh->slots[0].mesh->renderGroup(rsm->static_mesh->slots[0].submesh_idx);
			}
			//rsm->static_mesh->slots[0].material->deactivateTextures();
		}
	}

	rt_black->clear(VEC4(0, 0, 0, 0));
	if (only_borders) {
		activateZ(ZCFG_OUTLINE, sencil_mask);
		activateBlend(BLENDCFG_ADDITIVE);
		//edge detection
		{
			
			//PROFILE_FUNCTION(("referred: mark detection " + tagstr).c_str());
			//CTraceScoped scope(("mark detection " + tagstr).c_str());
			PROFILE_FUNCTION("referred: mark detection ");
			CTraceScoped scope("mark detection ");

			// Activar el rt para pintar las luces...

			ID3D11RenderTargetView* rts[3] = {
				rt_black->getRenderTargetView()
				,	nullptr   // remove the other rt's from the pipeline
				,	nullptr
			};
			// Y el ZBuffer del backbuffer principal

			Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

			//activateZ(ZCFG_ALL_DISABLED);

			auto tech = Resources.get("global_color.tech")->as<CRenderTechnique>();

			drawFullScreen(rt_final, tech);
			//rt_black->clear(VEC4(0, 0, 0, 1)); //we dont care about that texture, clean black texture
			CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);

			//Render.activateBackBuffer();

			//drawFullScreen(rt_black);
		}
	}

	{
		//PROFILE_FUNCTION(("referred: edge detection " + tagstr).c_str());
		//CTraceScoped scope(("edge detection " + tagstr).c_str());
		PROFILE_FUNCTION("referred: edge detection ");
		CTraceScoped scope("edge detection ");

		ID3D11RenderTargetView* rts[3] = {
			rt_black->getRenderTargetView()
			,	nullptr   // remove the other rt's from the pipeline
			,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		
		if(only_borders)	activateBlend(BLENDCFG_SUBSTRACT);	//to make outlines
		if(!only_borders)	activateBlend(BLENDCFG_ADDITIVE);
		activateZ(ZCFG_ALL_DISABLED);

		auto tech = Resources.get("global_color.tech")->as<CRenderTechnique>();
		tech->activate();

		auto hs = tags_manager.getHandlesByTag(tagstr);
		//Resources.get("shadow_gen_skin.tech")->as<CRenderTechnique>()->activate();

		bool mesh_uploaded = false;

		for (CHandle h : hs) {
			CEntity* e = h;
			if (!e) continue;
			if (!isInRoom(h)) continue;
			TCompRenderStaticMesh *rsm = e->get<TCompRenderStaticMesh>();
			TCompTransform *c_tmx = e->get<TCompTransform>();
			if (!c_tmx || !rsm) continue;

			//push scaled matrix
			activateWorldMatrix(c_tmx->asMatrix());

			if (!mesh_uploaded) {
				rsm->static_mesh->slots[0].mesh->activate();
				mesh_uploaded = true;
			}

			if (use_skeleton) {
				const TCompSkeleton* comp_skel = e->get<TCompSkeleton>();
				assert(comp_skel);
				comp_skel->uploadBonesToCteShader();
			}

			//rsm->static_mesh->slots[0].mesh->render();
			rsm->static_mesh->slots[0].mesh->renderGroup(rsm->static_mesh->slots[0].submesh_idx);
			//rsm->static_mesh->slots[0].material->deactivateTextures();
		}

		//drawFullScreen(rt_final, tech);
		//rt_black->clear(VEC4(0, 0, 0, 1)); //we dont care about that texture, clean black texture
		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}
	{
		ID3D11RenderTargetView* rts[3] = {
			rt_selfIlum->getRenderTargetView()
			,	nullptr   // remove the other rt's from the pipeline
			,	nullptr
		};
		// Y el ZBuffer del backbuffer principal

		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
		activateBlend(BLENDCFG_ADDITIVE);
		//activateZ(ZCFG_ALL_DISABLED);
		drawFullScreen(rt_black);	//rt_black contain outlined meshes

		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}
	//Render.activateBackBuffer();
}

void CRenderDeferredModule::renderDetails(CRenderTechnique::eCategory type) {
	// -------------------------
	// Activar mis multiples render targets
	ID3D11RenderTargetView* rts[5] = {
		rt_albedos->getRenderTargetView()
		,	rt_normals->getRenderTargetView()
		,	rt_depths->getRenderTargetView()
		,   rt_glossiness->getRenderTargetView()
		,   rt_specular->getRenderTargetView()
	};

	//Render.activateBackBuffer();
	SetOutputDeferred();

	Render.ctx->OMSetRenderTargets(5, rts, Render.depth_stencil_view);

	activateZ(ZCFG_Z_TEST_LESS_EQUAL);
	activateBlend(BLENDCFG_COMBINATIVE);

	RenderManager.renderAll(h_camera, type, SBB::readSala());
	//RenderManager.renderAll(h_camera, CRenderTechnique::TRANSPARENT_OBJS);
}

void CRenderDeferredModule::uploadConstantsGPU() {
	CEntity* e_player = tags_manager.getFirstHavingTag("raijin");
	if (e_player) {
		player_controller* pc = e_player->get<player_controller>();
		if (pc) {
			shader_ctes_globals.polarity = pc->GetPolarityInt();
		}
	}
	shader_ctes_globals.uploadToGPU();
}

void CRenderDeferredModule::applyPostFX() {
	CTraceScoped scope("applyPostFX");
	CEntity* e_camera = h_camera;
	if (!e_camera)
		return;

	CTexture* next_step = rt_selfIlum;

	TCompRenderGlow* glow = e_camera->get< TCompRenderGlow >();
	if (glow)
		next_step = glow->apply(next_step);
	else
		return;

	// ------------------------
	//Render.activateBackBuffer();
	SetOutputDeferred();

	//activateZ(ZCFG_ALL_DISABLED);

	activateBlend(BLENDCFG_ADDITIVE);

	drawFullScreen(next_step);

	activateZ(ZCFG_DEFAULT);
}

void CRenderDeferredModule::renderUI() {
	PROFILE_FUNCTION("renderUI");
	CTraceScoped scope("renderUI");
	activateZ(ZCFG_ALL_DISABLED);
	//activateZ(ZCFG_DEFAULT);
	//activateBlend(BLENDCFG_DEFAULT);
	activateBlend(BLENDCFG_COMBINATIVE);

	//Render.clearMainZBuffer();

	CHandle h_ui_camera = tags_manager.getFirstHavingTag(getID("ui_camera"));
	if (!h_ui_camera.isValid())
		return;
	CEntity* e_ui_camera = h_ui_camera;

	TCompCamera* ortho = e_ui_camera->get<TCompCamera>();
	//ortho->setOrtho(xres, yres, ortho->getZNear(), ortho->getZFar());
	//ortho->setOrtho(10.f, 10.f, ortho->getZNear(), ortho->getZFar(), getAspectRatio());
	activateCamera(ortho);

	/*

	CCamera ortho;
	ortho.setProjection(deg2rad(60), -1.f, 1.f);
	ortho.setOrtho(xres, yres);
	activateCamera(&ortho);

	//
	auto tech = Resources.get("ui.tech")->as<CRenderTechnique>();
	tech->activate();

	auto texture = Resources.get("textures/logo.dds")->as<CTexture>();
	texture->activate(TEXTURE_SLOT_DIFFUSE);

	MAT44 world;
	world =
	  MAT44::CreateTranslation(1, 1, 0)
	  * MAT44::CreateScale(0.5, 0.5, 1)
	  * MAT44::CreateScale(512, 400, 1)
	  * MAT44::CreateTranslation(0, 400, 0)
	  ;

	world =
	  MAT44::CreateTranslation(1, 1, 0)
	  * MAT44::CreateScale(0.5, 0.5, 1)
	  * MAT44::CreateScale(2, 2, 1)
	  * MAT44::CreateTranslation(1, 1, 0)
	  ;
	activateWorldMatrix(world);

	auto mesh = Resources.get("unitQuadXY.mesh")->as<CMesh>();
	mesh->activateAndRender();
	*/
	//Render.activateBackBuffer();
	RenderManager.renderAll(h_ui_camera, CRenderTechnique::UI_OBJS, SBB::readSala());
}

float CRenderDeferredModule::getAspectRatio()
{
	return (float)xres / (float)yres;
}
