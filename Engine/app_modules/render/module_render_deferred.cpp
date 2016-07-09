#include "mcv_platform.h"
#include "module_render_deferred.h"
#include "camera/camera.h"
#include "components/entity_tags.h"
#include "components/entity.h"
#include "components/comp_camera_main.h"
#include "components/comp_light_dir.h"
#include "components/comp_light_dir_shadows.h"
#include "components/comp_light_point.h"
#include "components/comp_light_fadable.h"
#include "render/render.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "render/draw_utils.h"
#include "components/comp_render_glow.h"

#include "components\comp_render_fade_screen.h"

#include "render/fx/GuardShots.h"

//Particles
#include "particles\particles_manager.h"

//POLARIZE
#include "components/comp_polarized.h"
#include "components\comp_transform.h"
#include "components\comp_render_static_mesh.h"
#include "render\static_mesh.h"
#include "components\comp_life.h"
#include "player_controllers\player_controller.h"

// ------------------------------------------------------
bool CRenderDeferredModule::start() {
	//xres = CApp::get().getXRes();
	//yres = CApp::get().getYRes();

	xres = Render.getXRes();
	yres = Render.getYRes();

	rt_albedos = new CRenderToTexture;
	rt_normals = new CRenderToTexture;
	rt_depths = new CRenderToTexture;
	rt_acc_light = new CRenderToTexture;
	rt_selfIlum = new CRenderToTexture;
	rt_selfIlum_blurred = new CRenderToTexture;
	rt_final = new CRenderToTexture;

	rt_specular = new CRenderToTexture;
	rt_shadows = new CRenderToTexture;

	//aux
	rt_data = new CRenderToTexture;
	rt_data2 = new CRenderToTexture;
	rt_black = new CRenderToTexture;
	rt_temp = new CRenderToTexture;

	//temp
	rt_selfIlum_int = new CRenderToTexture;
	rt_selfIlum_blurred_int = new CRenderToTexture;

	if (!rt_specular->createRT("rt_specular", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
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
	if (!rt_temp->createRT("rt_temp", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;
	if (!rt_shadows->createRT("rt_shadows", xres, yres, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN))
		return false;

	//
	acc_light_points = Resources.get("deferred_lights_point.tech")->as<CRenderTechnique>();
	assert(acc_light_points && acc_light_points->isValid());

	acc_light_directionals = Resources.get("deferred_lights_dir.tech")->as<CRenderTechnique>();
	assert(acc_light_directionals && acc_light_directionals->isValid());

	//blur_tech = Resources.get("blur_glow.tech")->as<CRenderTechnique>();

	acc_light_directionals_shadows = Resources.get("deferred_lights_dir_shadows.tech")->as<CRenderTechnique>();
	assert(acc_light_directionals_shadows && acc_light_directionals_shadows->isValid());

	//  unit_sphere = Resources.get("meshes/engine/unit_sphere.mesh")->as<CMesh>();
	unit_sphere = Resources.get("unitQuadXY.mesh")->as<CMesh>();
	assert(unit_sphere && unit_sphere->isValid());
	unit_cube = Resources.get("meshes/engine/unit_frustum.mesh")->as<CMesh>();
	assert(unit_cube && unit_cube->isValid());


  Resources.get("textures/general/noise.dds")->as<CTexture>()->activate(TEXTURE_SLOT_NOISE );

  //hatching texture
  Resources.get("textures/hatching/hatch_0.dds")->as<CTexture>()->activate(TEXTURE_SLOT_HATCHING);

  //tests hatching
//#ifdef _DEBUG
  //Resources.get("textures/hatching/hatching_tileable.dds")->as<CTexture>()->activate(TEXTURE_SLOT_HATCHING_TEST1);
  //Resources.get("textures/hatching/hatching_tileable_prueba_plugin.dds")->as<CTexture>()->activate(TEXTURE_SLOT_HATCHING_TEST2);
//#endif
  
  Resources.get("textures/hatching/hatching_tileable_ALPHAS.dds")->as<CTexture>()->activate(63);
  Resources.get("textures/hatching/hatching_tileable_prueba_plugin.dds")->as<CTexture>()->activate(64);


  Resources.get("textures/ramps/rampa_prueba.dds")->as<CTexture>()->activate(TEXTURE_SLOT_RAMP);

  shader_ctes_hatching.edge_lines_detection = 0.02f;
  shader_ctes_hatching.frequency_offset = 8.0f;
  shader_ctes_hatching.intensity_sketch = 0.2f;
  shader_ctes_hatching.rim_strenght = 1.0f;
  shader_ctes_hatching.specular_strenght = 50.0f;
  shader_ctes_hatching.diffuse_strenght = 1.0f;
  shader_ctes_hatching.frequency_texture = 10.0f;
  shader_ctes_hatching.color_ramp = 0.0f;
  shader_ctes_hatching.specular_force = 0.2f;
  shader_ctes_hatching.rim_specular = 1.5f;

  shader_ctes_globals.world_time = 0.f;
  shader_ctes_globals.xres = xres;
  shader_ctes_globals.yres = yres;
  shader_ctes_globals.strenght_polarize = 1.0f / 5.0f;

  shader_ctes_hatching.uploadToGPU();

  return true;
}

// ------------------------------------------------------
void CRenderDeferredModule::stop() {
	//RenderManager.clear();
}

// ------------------------------------------------------
void CRenderDeferredModule::update(float dt) {
	shader_ctes_globals.world_time += dt;
}

// ------------------------------------------------------
void CRenderDeferredModule::renderGBuffer() {
	PROFILE_FUNCTION("GBuffer");
	CTraceScoped scope("GBuffer");
	h_camera = tags_manager.getFirstHavingTag(getID("camera_main"));

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
	ID3D11RenderTargetView* rts[4] = {
	  rt_albedos->getRenderTargetView()
	  ,	rt_normals->getRenderTargetView()
	  ,	rt_depths->getRenderTargetView()
	  ,   rt_selfIlum->getRenderTargetView()
	};
	// Y el ZBuffer del backbuffer principal
	Render.ctx->OMSetRenderTargets(4, rts, Render.depth_stencil_view);
	rt_albedos->activateViewport();
	// Clear de los render targets y el ZBuffer
	rt_albedos->clear(VEC4(1, 0, 0, 1));
	rt_normals->clear(VEC4(0, 1, 0, 1));
	rt_selfIlum->clear(VEC4(0, 0, 0, 1));
	rt_depths->clear(VEC4(1, 1, 1, 1));
	rt_final->clear(VEC4(0, 0, 0, 0));
	Render.clearMainZBuffer();

	rt_acc_light->clear(VEC4(0, 0, 0, 1));
	rt_shadows->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_blurred->clear(VEC4(0, 0, 0, 1));

	// Activa la ctes del object
	activateWorldMatrix(MAT44::Identity);

	// Mandar a pintar los 'solidos'
	if (h_camera.isValid())
		RenderManager.renderAll(h_camera, CRenderTechnique::SOLID_OBJS);

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
		c->activate();
		// Pintar la mesh que hemos activado hace un momento
		mesh->render();
	});

	CTexture::deactivate(TEXTURE_SLOT_SHADOWMAP);
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


	Render.activateBackBuffer();	//reset size to default

	// Y el ZBuffer del backbuffer principal
	Render.ctx->OMSetRenderTargets(3, rts, nullptr);

	rt_albedos->activate(TEXTURE_SLOT_DIFFUSE);
	rt_acc_light->activate(TEXTURE_SLOT_ENVIRONMENT);
	rt_selfIlum->activate(TEXTURE_SLOT_SELFILUM);
	rt_depths->activate(TEXTURE_SLOT_DEPTHS);
	rt_normals->activate(TEXTURE_SLOT_NORMALS);


	activateZ(ZCFG_ALL_DISABLED);
  activateBlend(BLENDCFG_DEFAULT);
  //auto tech = Resources.get("deferred_add_ambient.tech")->as<CRenderTechnique>();
  drawFullScreen(rt_acc_light);

  activateZ(ZCFG_DEFAULT);

  CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
  CTexture::deactivate(TEXTURE_SLOT_NORMALS);
  CTexture::deactivate(TEXTURE_SLOT_SELFILUM);
  CTexture::deactivate(TEXTURE_SLOT_ENVIRONMENT);
  CTexture::deactivate(TEXTURE_SLOT_DEPTHS);
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

// ----------------------------------------------
void CRenderDeferredModule::renderAccLight() {
	PROFILE_FUNCTION("renderAccLight");
	CTraceScoped scope("renderAccLight");

	// Activar el rt para pintar las luces...
	ID3D11RenderTargetView* rts[3] = {
	  rt_acc_light->getRenderTargetView()
	  ,  rt_specular->getRenderTargetView()
	  ,  rt_shadows->getRenderTargetView()
	};
	// Y el ZBuffer del backbuffer principal
	Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

  // Activar las texturas del gbuffer en la pipeline para
  // que se puedan acceder desde los siguientes shaders
  //rt_albedos->activate(TEXTURE_SLOT_DIFFUSE);	//activated on addAmbientPass
  rt_depths->activate(TEXTURE_SLOT_DEPTHS);
  rt_normals->activate(TEXTURE_SLOT_NORMALS);

  //rt_acc_light->clear(VEC4(0, 0, 0, 1));

  activateBlend(BLENDCFG_DEFAULT);
  addAmbientPass();

  activateBlend(BLENDCFG_ADDITIVE);
  activateZ(ZCFG_LIGHTS_CONFIG);
  //activateRS(RSCFG_INVERT_CULLING);
  addPointLights();

	activateZ(ZCFG_LIGHTS_CONFIG);
	//activateRS(RSCFG_INVERT_CULLING);
	addDirectionalLights();

  activateRS(RSCFG_DEFAULT);
  addDirectionalLightsShadows();

	activateRS(RSCFG_DEFAULT);
	activateZ(ZCFG_DEFAULT);
	activateBlend(BLENDCFG_DEFAULT);

	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	CTexture::deactivate(TEXTURE_SLOT_NORMALS);
	CTexture::deactivate(TEXTURE_SLOT_DEPTHS);
}

// ----------------------------------------------
void CRenderDeferredModule::generateShadowMaps() {
	PROFILE_FUNCTION("generateShadowMaps");
	CTraceScoped scope("generateShadowMaps");

	// Llamar al metodo generateShadowMap para todas los components de tipo dir_shadows
	getHandleManager<TCompLightDirShadows>()->onAll(&TCompLightDirShadows::generateShadowMap);
}

void CRenderDeferredModule::RenderPolarizedPP(int pol, const VEC4& color) {
	shader_ctes_globals.global_color = color;
	shader_ctes_globals.uploadToGPU();

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

		auto tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
		tech->activate();

		getHandleManager<TCompPolarized>()->each([pol](TCompPolarized* c) {
			if (c->force.polarity == pol) {	//render polarity designed only
				CEntity *e = CHandle(c).getOwner();
				TCompRenderStaticMesh *rsm = e->get<TCompRenderStaticMesh>();
				TCompTransform *c_tmx = e->get<TCompTransform>();

				activateWorldMatrix(c_tmx->asMatrix());

				//rsm->static_mesh->slots[0].material->activateTextures();
				rsm->static_mesh->slots[0].mesh->activateAndRender();

				//rsm->static_mesh->slots[0].material->deactivateTextures();
			}
		});
	}

	//edge detection
	{
		PROFILE_FUNCTION("referred: edge detection");
		CTraceScoped scope("edge detection final");

		// Activar el rt para pintar las luces...

		ID3D11RenderTargetView* rts[3] = {
		  rt_data->getRenderTargetView()
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
		//rt_black->clear(VEC4(0, 0, 0, 1)); //we dont care about that texture, clean black texture
		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}
}

void CRenderDeferredModule::GlowEdgesInt() {
	{
		PROFILE_FUNCTION("referred: GlowEdges");
		CTraceScoped scope("glow edges");

		ID3D11RenderTargetView* rts1[3] = {
		  rt_selfIlum_int->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		// Y el ZBuffer del backbuffer principal
		Render.ctx->OMSetRenderTargets(3, rts1, Render.depth_stencil_view);

		auto tech = Resources.get("solid_PP.tech")->as<CRenderTechnique>();
		drawFullScreen(rt_data, tech);

		blurEffectLights(true);

		tech = Resources.get("solid_PP.tech")->as<CRenderTechnique>();

		activateBlend(BLENDCFG_COMBINATIVE);
		Render.activateBackBuffer();				//render on screen
		activateZ(ZCFG_ALL_DISABLED);
		drawFullScreen(rt_selfIlum_blurred_int, tech);
		activateBlend(BLENDCFG_DEFAULT);
	}
}

void CRenderDeferredModule::GlowEdges() {
	{
		PROFILE_FUNCTION("referred: GlowEdges");
		CTraceScoped scope("glow edges");

		ID3D11RenderTargetView* rts1[3] = {
		  rt_selfIlum->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		// Y el ZBuffer del backbuffer principal
		Render.ctx->OMSetRenderTargets(3, rts1, Render.depth_stencil_view);

		auto tech = Resources.get("solid_PP.tech")->as<CRenderTechnique>();
		activateBlend(BLENDCFG_COMBINATIVE);
		drawFullScreen(rt_data2, tech);

		activateBlend(BLENDCFG_DEFAULT);
	}
}

void CRenderDeferredModule::ShootGuardRender() {
	// Fx
	{
		PROFILE_FUNCTION("referred: mask laser");
		CTraceScoped scope("mask laser");
		rt_temp->clear(VEC4(0, 0, 0, 0));
		//activateZ(ZCFG_DEFAULT);
		activateZ(ZCFG_MASK_NUMBER, 3);
		activateBlend(BLENDCFG_ADDITIVE);
		ID3D11RenderTargetView* rts[3] = {
		  rt_temp->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		auto tech = Resources.get("solid_PSnull.tech")->as<CRenderTechnique>();
		tech->activate();

		//Render.activateBackBuffer();
		//activateZ(ZCFG_DEFAULT);
		ShootManager::renderAll();
	}

	{
		PROFILE_FUNCTION("referred: laser");
		CTraceScoped scope("laser");

		ID3D11RenderTargetView* rts[3] = {
		rt_temp->getRenderTargetView()
		,	nullptr   // remove the other rt's from the pipeline
		,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
		//activateBlend(BLENDCFG_ADDITIVE);

		activateZ(ZCFG_OUTLINE, 3);

		auto tech = Resources.get("test_shoot_w.tech")->as<CRenderTechnique>();
		tech->activate();

		rt_temp->clear(VEC4(0, 0, 0, 1));
		rt_data2->clear(VEC4(0, 0, 0, 1));
		drawFullScreen(rt_data2, tech);
		//activateZ(ZCFG_DEFAULT);
		//ShootManager::renderAll();
		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}

	{
		PROFILE_FUNCTION("referred: add laser");
		CTraceScoped scope("add laser");
		Render.activateBackBuffer();
		rt_data2->clear(VEC4(0, 0, 0, 0));
		ID3D11RenderTargetView* rts[3] = {
		  rt_data2->getRenderTargetView()
		  ,	nullptr   // remove the other rt's from the pipeline
		  ,	nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, nullptr);
		//activateBlend(BLENDCFG_ADDITIVE);

		activateZ(ZCFG_ALL_DISABLED);
		activateBlend(BLENDCFG_DEFAULT);
		auto tech = Resources.get("test_shoot.tech")->as<CRenderTechnique>();
		tech->activate();

		drawFullScreen(rt_temp, tech);

		CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);
	}

	if (GameController->GetGameState() == CGameController::RUNNING)ShootManager::shots.clear();
}

// ----------------------------------------------
void CRenderDeferredModule::render() {
	//Render.clearMainZBuffer();
	rt_black->clear(VEC4(0, 0, 0, 1));
	rt_data->clear(VEC4(0, 0, 0, 0));
	rt_specular->clear(VEC4(0, 0, 0, 0));

	generateShadowMaps();

	rt_data2->clear(VEC4(0, 0, 0, 0));

	rt_selfIlum->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_int->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_blurred->clear(VEC4(0, 0, 0, 0));
	rt_selfIlum_blurred_int->clear(VEC4(0, 0, 0, 0));

	shader_ctes_globals.uploadToGPU();
	renderGBuffer();
	renderDetails();
	renderAccLight();
	
	CTexture* blurred_shadows = rt_shadows;

	CEntity* e_camera = h_camera;
	if (!e_camera)
		return;

	//blur shadows
	TCompRenderGlow* glow = e_camera->get< TCompRenderGlow >();
	if (glow)
		blurred_shadows = glow->apply(blurred_shadows);

	//blurEffectLights();

	 //render_particles_instanced.render();
	g_particlesManager->renderParticles();   //render all particles systems

	FinalRender();

	rt_depths->activate(TEXTURE_SLOT_DEPTHS);
	Render.activateBackBuffer();
	activateZ(ZCFG_ALL_DISABLED);

	//AA cutre, only objects near camera
	/*auto tech = Resources.get("aa_tech.tech")->as<CRenderTechnique>();
	drawFullScreen(rt_final, tech);*/

	drawFullScreen(rt_final);

	activateBlend(BLENDCFG_COMBINATIVE);
	rt_specular->activate(TEXTURE_SLOT_SPECULAR_GL);

	blurred_shadows->activate(TEXTURE_SLOT_SHADOWS);
	rt_normals->activate(TEXTURE_SLOT_NORMALS);
	//rt_shadows->activate(TEXTURE_SLOT_SHADOWS);
	auto tech = Resources.get("hatching.tech")->as<CRenderTechnique>();
	drawFullScreen(rt_final, tech);
	CTexture::deactivate(TEXTURE_SLOT_SHADOWS);

	rt_depths->activate(TEXTURE_SLOT_DEPTHS);
	

	activateBlend(BLENDCFG_SUBSTRACT);
	tech = Resources.get("edgeDetection.tech")->as<CRenderTechnique>();
	drawFullScreen(rt_final, tech);

	activateBlend(BLENDCFG_DEFAULT);
	activateZ(ZCFG_DEFAULT);

	/*if (GameController->GetFxPolarize()) {
	  RenderPolarizedPP(MINUS, VEC4(1.0f, 0.3f, 0.3f, 1.0f));
	  RenderPolarizedPP(PLUS, VEC4(0.3f, 0.3f, 1.0f, 1.0f));
	}*/

	activateZ(ZCFG_DEFAULT);

	ShootGuardRender();

	activateZ(ZCFG_ALL_DISABLED);

	/*if (GameController->GetFxGlow()) {
	  GlowEdgesInt();
	  GlowEdges();
	}*/

	
	CTexture::deactivate(TEXTURE_SLOT_SHADOWS);
	CTexture::deactivate(TEXTURE_SLOT_SPECULAR_GL);
	CTexture::deactivate(TEXTURE_SLOT_NORMALS);

	Render.activateBackBuffer();
	activateZ(ZCFG_DEFAULT);

	applyPostFX();

	// Mandar a pintar los 'transparentes'
	rt_depths->activate(TEXTURE_SLOT_DEPTHS);
	RenderManager.renderAll(h_camera, CRenderTechnique::TRANSPARENT_OBJS);
	CTexture::deactivate(TEXTURE_SLOT_DEPTHS);

	CTexture::deactivate(TEXTURE_SLOT_DIFFUSE);

	CEntity* ec = h_camera;
	TCompFadeScreen* e = ec->get<TCompFadeScreen>();
	if (e) {
		activateBlend(BLENDCFG_COMBINATIVE);
		e->render();
		activateBlend(BLENDCFG_DEFAULT);
	}

	renderUI();

	// Leave the 3D Camera active
	activateRenderCamera3D();
}

void CRenderDeferredModule::renderDetails() {
	// -------------------------
	// Activar mis multiples render targets
	ID3D11RenderTargetView* rts[3] = {
		rt_albedos->getRenderTargetView()
		,	rt_normals->getRenderTargetView()
		,	nullptr
	};

	Render.activateBackBuffer();

	Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

	activateZ(ZCFG_Z_TEST_LESS_EQUAL);
	activateBlend(BLENDCFG_COMBINATIVE);

	RenderManager.renderAll(h_camera, CRenderTechnique::DETAIL_OBJS);
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
	Render.activateBackBuffer();

	//activateZ(ZCFG_ALL_DISABLED);

	activateBlend(BLENDCFG_ADDITIVE);

	drawFullScreen(next_step);
	activateZ(ZCFG_DEFAULT);
}

void CRenderDeferredModule::renderUI() {
	PROFILE_FUNCTION("renderUI");
	CTraceScoped scope("renderUI");
	activateZ(ZCFG_ALL_DISABLED);
	activateBlend(BLENDCFG_DEFAULT);

	CHandle h_ui_camera = tags_manager.getFirstHavingTag(getID("ui_camera"));
	if (!h_ui_camera.isValid())
		return;
	CEntity* e_ui_camera = h_ui_camera;

	TCompCamera* ortho = e_ui_camera->get<TCompCamera>();
	//ortho->setOrtho(xres, yres, ortho->getZNear(), ortho->getZFar());
	ortho->setOrtho(10, 10, ortho->getZNear(), ortho->getZFar());
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
	RenderManager.renderAll(h_ui_camera, CRenderTechnique::UI_OBJS);
}