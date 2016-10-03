#include "mcv_platform.h"
#include "comp_render_glow.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "windows/app.h"

struct TBlurStep {
	CRenderToTexture* rt_half_y;
	CRenderToTexture* rt_output;    // half x & y

	int xres, yres;
	const CRenderTechnique* tech;
	const CMesh*            mesh;

	// ---------------------
	bool create(const char* name, int in_xres, int in_yres) {
		xres = in_xres;
		yres = in_yres;
		rt_half_y = new CRenderToTexture();
		std::string sname = std::string(name) + "_y";
		bool is_ok = rt_half_y->createRT(sname.c_str(), xres, yres / 2, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN);
		assert(is_ok);

		sname = std::string(name) + "_xy";
		rt_output = new CRenderToTexture();
		is_ok = rt_output->createRT(sname.c_str(), xres / 2, yres / 2, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN);
		assert(is_ok);

		tech = Resources.get("blur.tech")->as<CRenderTechnique>();
		mesh = Resources.get("unitQuadXY.mesh")->as<CMesh>();

		return is_ok;
	}

	// ---------------------
	void applyBlur(float dx, float dy, const CRenderTechnique* technique) {
		shader_ctes_blur.blur_step.x = (float)dx / (float)xres;
		shader_ctes_blur.blur_step.y = (float)dy / (float)yres;
		// Box filter
		shader_ctes_blur.uploadToGPU();

		if (technique)
			technique->activate();
		else
			tech->activate();

		mesh->activateAndRender();
	}

	// ---------------------
	CTexture* apply(CTexture* input, float global_distance, VEC4 distances, VEC4 weights, const CRenderTechnique* technique) {
		float normalization_factor =
			1 * weights.x
			+ 2 * weights.y
			+ 2 * weights.z
			+ 2 * weights.w
			;
		shader_ctes_blur.blur_w.x = weights.x / normalization_factor;
		shader_ctes_blur.blur_w.y = weights.y / normalization_factor;
		shader_ctes_blur.blur_w.z = weights.z / normalization_factor;
		shader_ctes_blur.blur_w.w = weights.w / normalization_factor;
		shader_ctes_blur.blur_d.x = distances.x;
		shader_ctes_blur.blur_d.y = distances.y;
		shader_ctes_blur.blur_d.z = distances.z;
		shader_ctes_blur.blur_d.w = distances.w;  // Not used

		auto tech = Resources.get("solid_textured.tech")->as<CRenderTechnique>();
		tech->activate();

		rt_half_y->activateRT();
		input->activate(TEXTURE_SLOT_DIFFUSE);
		applyBlur(0, global_distance, technique);

		rt_output->activateRT();
		rt_half_y->activate(TEXTURE_SLOT_DIFFUSE);
		applyBlur(global_distance, 0, technique);

		return rt_output;
	}
};

// ---------------------
void TCompRenderGlow::renderInMenu() {
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::DragInt("# Steps", &nactive_steps, 0.1f, 0, steps.size());
	ImGui::InputFloat("Weights Center", &weights.x);
	ImGui::InputFloat("Weights 1st", &weights.y);
	ImGui::InputFloat("Weights 2nd", &weights.z);
	ImGui::InputFloat("Weights 3rd", &weights.w);
	if (ImGui::SmallButton("box")) {
		distance_factors = VEC4(1, 2, 3, 4);
		weights = VEC4(1, 1, 1, 1);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("gauss")) {
		weights = VEC4(70, 56, 28, 8);
		distance_factors = VEC4(1, 2, 3, 4);
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("linear")) {
		// This is a 5 taps kernel (center + 2 taps on each side)
		// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
		weights = VEC4(0.2270270270f, 0.3162162162f, 0.0702702703f, 0.f);
		distance_factors = VEC4(1.3846153846f, 3.2307692308f, 0.f, 0.f);
	}
	if (ImGui::SmallButton("Preset1")) {
		weights = VEC4(70, 56, 28, 8);
		distance_factors = VEC4(1, 2, 3, 4);
		global_distance = 2.7f;
		//nactive_steps = 3;
	}
	if (ImGui::SmallButton("Preset2")) {
		weights = VEC4(70, 56, 28, 8);
		distance_factors = VEC4(1, 2, 3, 4);
		global_distance = 2.0f;
		//nactive_steps = 2;
	}
	ImGui::DragFloat("global_distance", &global_distance, 0.1f, 0.1f, 8.0f);
	ImGui::InputFloat("Distance 2nd Tap", &distance_factors.x);
	ImGui::InputFloat("Distance 3rd Tap", &distance_factors.y);
	ImGui::InputFloat("Distance 4th Tap", &distance_factors.z);
	if (ImGui::SmallButton("Default Distances")) {
		distance_factors = VEC4(1, 2, 3, 4);
	}
}

bool TCompRenderGlow::initValues() {
	enabled = true;
	global_distance =  1.0f;
	distance_factors = VEC4(1, 2, 3, 4);

	weights.x = 1.f;
	weights.y = 1.f;
	weights.z = 1.f;
	weights.w = 1.f;

	bool is_ok = true;
	int nsteps = 2;
	int xres = CApp::get().getXRes();
	int yres = CApp::get().getYRes();
	static int g_blur_counter = 0;
	for (int i = 0; i < nsteps; ++i) {
		TBlurStep* s = new TBlurStep;

		char blur_name[64];
		sprintf(blur_name, "Blur_%02d", g_blur_counter);
		g_blur_counter++;

		is_ok &= s->create(blur_name, xres, yres);
		assert(is_ok);
		steps.push_back(s);
		xres /= 2;
		yres /= 2;
	}

	nactive_steps = steps.size();
	return is_ok;
}

bool TCompRenderGlow::load(MKeyValue& atts) {
	enabled = atts.getBool("enabled", true);
	global_distance = atts.getFloat("global_distance", 1.0f);
	distance_factors = VEC4(1, 2, 3, 4);

	weights.x = atts.getFloat("w0", 1.f);
	weights.y = atts.getFloat("w1", 1.f);
	weights.z = atts.getFloat("w2", 1.f);
	weights.w = atts.getFloat("w3", 1.f);

	if (atts.getBool("box_filter", false))
		weights = VEC4(1, 1, 1, 1);
	else if (atts.getBool("gauss_filter", false))
		weights = VEC4(70, 56, 28, 8);
	/*
				  1
				1   1
			  1   2   1
			1   3   3   1
		  1   4   6   4   1
		1   5   10 10   5   1
	  1   6   15  20  15  6   1
	1   7   21  35  35  21  7   1
  1   8   28  56  70  56  28  8   1   <-- Four taps, discard the last 1
  */

	bool is_ok = true;
	int nsteps = atts.getInt("max_steps", 2);
	int xres = CApp::get().getXRes();
	int yres = CApp::get().getYRes();
	static int g_blur_counter = 0;
	for (int i = 0; i < nsteps; ++i) {
		TBlurStep* s = new TBlurStep;

		char blur_name[64];
		sprintf(blur_name, "Blur_%02d", g_blur_counter);
		g_blur_counter++;

		is_ok &= s->create(blur_name, xres, yres);
		assert(is_ok);
		steps.push_back(s);
		xres /= 2;
		yres /= 2;
	}

	nactive_steps = atts.getInt("active_steps", steps.size());
	return is_ok;
}

CTexture* TCompRenderGlow::apply(CTexture* input, const CRenderTechnique* technique) {
	if (!enabled)
		return input;
	CTraceScoped scope("CompGlow");

	CTexture* output = input;
	int nsteps_to_apply = nactive_steps;
	for (auto s : steps) {
		if (--nsteps_to_apply < 0)
			break;
		output = s->apply(input, global_distance, distance_factors, weights, technique);
		input = output;
	}

	return output;
}