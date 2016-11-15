#include "mcv_platform.h"
#include "comp_light_dir.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "app_modules/imgui/module_imgui.h"

bool TCompLightDir::debug_render = false;

bool TCompLightDir::load(MKeyValue& atts) {
	TCompCamera::load(atts);
	color = atts.getQuat("color");
	shadow_intensity = atts.getFloat("shadow_intensity", 1.0f);
	if(color.w > 0) light_mask_path = atts.getString("light_mask", "textures/dir_shader_circular.dds");
	else {
		light_mask_path = atts.getString("light_mask", "textures/dir_textura_1p_negro_margen.dds");
	}
	light_mask = Resources.get(light_mask_path.c_str())->as<CTexture>();
	return true;
}
bool TCompLightDir::save(std::ofstream& os, MKeyValue& atts) {
	atts.put("color", color);
	atts.put("shadow_intensity", shadow_intensity);
	atts.put("znear", getZNear());
	atts.put("zfar", getZFar());
	atts.put("fov", rad2deg(getFov()));
	atts.put("light_mask", light_mask_path);
	return true;
}
void TCompLightDir::render() const {
	if (debug_render) {
		PROFILE_FUNCTION("TCompLight render");
		auto axis = Resources.get("frustum.mesh")->as<CMesh>();
		shader_ctes_object.World = getViewProjection().Invert();
		shader_ctes_object.uploadToGPU();
		axis->activateAndRender();
	}
}

void TCompLightDir::renderInMenu() {
	TCompCamera::renderInMenu();
	ImGui::ColorEdit3("Color", &color.x);
	ImGui::DragFloat("Intensity", &color.w);
	ImGui::DragFloat("Shadow Intensity", &shadow_intensity);
	ImGui::Text("Light mask path: %s", light_mask_path.c_str());
	renderLightMaskInMenu();
}
void TCompLightDir::renderLightMaskInMenu()
{
	if (ImGui::Button("Change light mask...")) {
		std::string newTextureFullPath = CImGuiModule::getFilePath();
		std::replace(newTextureFullPath.begin(), newTextureFullPath.end(), '\\', '/'); // replace all 'x' to 'y'
		std::string delimiter = "bin/data/";
		auto pos = newTextureFullPath.find(delimiter);
		if (pos != std::string::npos) {
			light_mask_path = newTextureFullPath.substr(pos + delimiter.length());
			light_mask = Resources.get(light_mask_path.c_str())->as<CTexture>();
		}
	}
}

void TCompLightDir::uploadShaderCtes(CEntity* e) {
	TCompTransform* trans = e->get<TCompTransform>();
	VEC3 lpos = trans->getPosition();
	shader_ctes_lights.LightWorldPos = VEC4(lpos);
	shader_ctes_lights.LightWorldFront = VEC4(trans->getFront());
	shader_ctes_lights.LightColor = this->color;
	shader_ctes_lights.LightViewProjection = this->getViewProjection();

	MAT44 offset = MAT44::CreateTranslation(0.5f, 0.5f, 0.f);
	MAT44 scale = MAT44::CreateScale(0.5f, -0.5f, 1.f);
	MAT44 tmx = scale * offset;
	shader_ctes_lights.LightViewProjectionOffset = shader_ctes_lights.LightViewProjection * tmx;
	shader_ctes_lights.LightOutRadius = this->getZFar();
	shader_ctes_lights.LightInRadius = this->getZNear();
	shader_ctes_lights.LightAspectRatio = this->getAspectRatio();
	shader_ctes_lights.LightCosFov = cosf(this->getFov());
	shader_ctes_lights.uploadToGPU();

	light_mask->activate(TEXTURE_SLOT_LIGHT_MASK);
}

void TCompLightDir::update(float dt) {
	CHandle owner = CHandle(this).getOwner();
	updateFromEntityTransform(owner);
}

void TCompLightDir::activate() {
	if (!enabled) return;
	activateWorldMatrix(getViewProjection().Invert());
	uploadShaderCtes(CHandle(this).getOwner());
}

//Editor
void TCompLightDir::start_editing() {
	if (original) delete original;
	original = new TCompLightDir;
	*original = *this;
	original->original = false;
	dbg("Test");
}
void TCompLightDir::cancel_editing() {
	if (!original) return;
	TCompLightDir * light_to_delete = original;
	*this = *original;
	if (light_to_delete) delete light_to_delete;
	if (original) delete original;
}

void TCompLightDir::reloadLightmap(std::string new_light_mask)
{
	light_mask_path = new_light_mask;
	light_mask = Resources.get(light_mask_path.c_str())->as<CTexture>();
}

TCompLightDir::~TCompLightDir()
{
	if (original) delete original;
}