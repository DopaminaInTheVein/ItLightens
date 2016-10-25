#include "mcv_platform.h"
#include "fx_ssao.h"
#include "render/draw_utils.h"
#include "resources\resources_manager.h"
#include "app_modules\render\module_render_deferred.h"
#include "components\comp_render_glow.h"
#include "handle\handle.h"
#include "components\entity.h"
#include "app_modules\render\module_render_postprocess.h"
#include "render\fx\fx_glow.h"

extern CRenderDeferredModule * render_deferred;
extern CRenderPostProcessModule * render_fx;

// ---------------------
void TRenderSSAO::renderInMenu() {
	
	TCompBasicFX::renderInMenu();
}

void TRenderSSAO::GetOcclusionTextue(CHandle h_camera, CRenderToTexture* rt_ssao, CRenderToTexture* rt_acc_light, CRenderToTexture* rt_shadows_gl) {
	tech->activate();
	GetTexture(h_camera, rt_ssao, rt_acc_light, rt_shadows_gl);
}

void TRenderSSAO::GetInvOcclusionTextue(CHandle h_camera, CRenderToTexture* rt_ssao, CRenderToTexture* rt_acc_light, CRenderToTexture* rt_shadows_gl) {
	tech_inv->activate();
	GetTexture(h_camera, rt_ssao, rt_acc_light, rt_shadows_gl);
}

void TRenderSSAO::GetTexture(CHandle h_camera, CRenderToTexture* rt_ssao, CRenderToTexture* rt_acc_light, CRenderToTexture* rt_shadows_gl) {
	rt_ssao->clear(VEC4(0, 0, 0, 0));

	if (!enabled) return;

	ID3D11RenderTargetView* rts[3] = {
		//Render.render_target_view
		rt_ssao->getRenderTargetView()
		,   nullptr
		,	nullptr
	};

	Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);
	activateBlend(BLENDCFG_DEFAULT);

	activateZ(ZCFG_ALL_DISABLED);

	drawFullScreen(rt_acc_light, tech);

	//Render.activateBackBuffer();
	render_deferred->SetOutputDeferred();
	//blur shadows
	CTexture *blurred_ssao = rt_ssao;
	CTexture *output;
	TRenderGlow* glow = render_fx->GetFX<TRenderGlow>("blur");
	//glow->
	if (glow)
		output = glow->apply(blurred_ssao);

	//reset size viewport
	render_deferred->SetOutputDeferred();
	activateZ(ZCFG_ALL_DISABLED);

	{
		ID3D11RenderTargetView* rts[3] = {
			rt_acc_light->getRenderTargetView()
			, rt_shadows_gl->getRenderTargetView()
			,   nullptr
		};
		Render.ctx->OMSetRenderTargets(3, rts, Render.depth_stencil_view);

		tech_multiple_targets->activate();

		activateBlend(BLENDCFG_SUBSTRACT);
		//activateBlend(BLENDCFG_DEFAULT);	//for testing only
		drawFullScreen(output, tech);
	}
	activateBlend(BLENDCFG_DEFAULT);
	//Render.activateBackBuffer();
	render_deferred->SetOutputDeferred();
}

void TRenderSSAO::ApplyFX() {
	//nothing to do
}

void TRenderSSAO::init() {
	enabled = true;
	tech = Resources.get("ssao.tech")->as<CRenderTechnique>();
	tech_inv = Resources.get("ssao_inv.tech")->as<CRenderTechnique>();
	tech_multiple_targets = Resources.get("solid_textured_multiple_outputs.tech")->as<CRenderTechnique>();

	shader_ctes_blur.ssao_intensity = 1.0f;
	shader_ctes_blur.ssao_iterations = 10.f;

	//test ssao
	shader_ctes_blur.ssao_test_intensity = 1.0f;
	shader_ctes_blur.ssao_scale = 1.0f;
	shader_ctes_blur.ssao_sample_rad = 0.4f;
	shader_ctes_blur.ssao_bias = 0.520;

	shader_ctes_blur.uploadToGPU();
}