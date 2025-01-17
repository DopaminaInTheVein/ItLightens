#include "mcv_platform.h"
#include "comp_light_dir_shadows.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "comp_room.h"
#include "player_controllers\player_controller_base.h"

bool TCompLightDirShadows::load(MKeyValue& atts) {
	TCompLightDir::load(atts);

	res = atts.getInt("resolution", 256);
	rt_shadows = new CRenderToTexture();
	// I don't need a color buffer, just the ZBuffer
	bool is_ok = rt_shadows->createRT("ShadowMap", Render.getXRes(), Render.getYRes(), DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_TYPELESS);
	setObjName(rt_shadows, "rt_shadows");
	assert(is_ok);

	rt_shadows_base = new CRenderToTexture();
	// I don't need a color buffer, just the ZBuffer
	is_ok = rt_shadows_base->createRT("ShadowMapBase", Render.getXRes(), Render.getYRes(), DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_TYPELESS);
	setObjName(rt_shadows, "rt_shadows_base");
	assert(is_ok);

	enabled = atts.getBool("enabled", true);
	generate_shadow = atts.getBool("gen_shadow", true);

	return is_ok;
}
bool TCompLightDirShadows::save(std::ofstream& os, MKeyValue& atts) {
	TCompLightDir::save(os, atts);
	atts.put("resolution", res);
	atts.put("gen_shadow", generate_shadow);
	return true;
}
void TCompLightDirShadows::update(float dt) {
	PROFILE_FUNCTION("shadows: update");
	CHandle owner = CHandle(this).getOwner();
	updateFromEntityTransform(owner);
}

void TCompLightDirShadows::uploadShaderCtes(CEntity* e) {
	TCompTransform* trans = e->get<TCompTransform>();
	if (!trans) return;
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

	shader_ctes_lights.LightShadowStrength = shadow_intensity;

	shader_ctes_lights.generate_shadows = generate_shadow;
	shader_ctes_lights.uploadToGPU();

	light_mask->activate(TEXTURE_SLOT_LIGHT_MASK);
}

void TCompLightDirShadows::activate() {
	PROFILE_FUNCTION("shadows: activate");
	if (!enabled) return;
	CHandle owner = CHandle(this).getOwner();
	activateWorldMatrix(getViewProjection().Invert());
	rt_shadows->getZTexture()->activate(TEXTURE_SLOT_SHADOWMAP);
	rt_shadows_base->getZTexture()->activate(TEXTURE_SLOT_SHADOWMAP_STATICS);
	uploadShaderCtes(owner);
}

void TCompLightDirShadows::generateStaticShadowMap() {
	if (!enabled)
		return;
	assert(rt_shadows_base);
	PROFILE_FUNCTION("shadows: generateShadowMapTest");
	// Vamos a empezar a pintar en el shadow map
	rt_shadows_base->clearZ();
	//rt_shadows->setZ(rt_shadows_base->getZ(), rt_shadows_base->getRT());
	//
	rt_shadows_base->activateRT();
	activateRS(RSCFG_SHADOWS);

	// Desde MI punto de vista, el pt de vista de la luz direccional
	// que genera sombras
	activateCamera(this);

	// activar la tech de shadow map generation
	Resources.get("shadow_gen.tech")->as<CRenderTechnique>()->activate();

	// Pintar los shadow casters
	RenderManager.renderStaticShadowCasters(CHandle(this).getOwner(), SBB::readSala());

	// activar la tech de shadow map generation
	//Resources.get("shadow_gen_skin.tech")->as<CRenderTechnique>()->activate();

	// Pintar los shadow casters
	//RenderManager.renderShadowCastersSkin(CHandle(this).getOwner(), SBB::readSala());

	activateRS(RSCFG_DEFAULT);
}

void TCompLightDirShadows::generateShadowMap() {
	if (!enabled)
		return;
	assert(rt_shadows);

	PROFILE_FUNCTION("shadows: generateShadowMap");

	// Vamos a empezar a pintar en el shadow map
	rt_shadows->clearZ();
	//rt_shadows->setZ(rt_shadows_base->getZ(), rt_shadows_base->getRT());
	//
	rt_shadows->activateRT();
	activateRS(RSCFG_SHADOWS);

	// Desde MI punto de vista, el pt de vista de la luz direccional
	// que genera sombras
	activateCamera(this);

	// activar la tech de shadow map generation
	Resources.get("shadow_gen.tech")->as<CRenderTechnique>()->activate();

	// Pintar los shadow casters
	RenderManager.renderShadowCasters(CHandle(this).getOwner(), SBB::readSala());

	// activar la tech de shadow map generation
	Resources.get("shadow_gen_skin.tech")->as<CRenderTechnique>()->activate();

	// Pintar los shadow casters
	RenderManager.renderShadowCastersSkin(CHandle(this).getOwner(), SBB::readSala());

	activateRS(RSCFG_DEFAULT);
}
void TCompLightDirShadows::destroy()
{
}

void TCompLightDirShadows::setNewFov(float fov_in_rads) {
	float fov_in_degs = rad2deg(fov_in_rads);
	setProjection(fov_in_rads, getZNear(), getZFar());
}
void TCompLightDirShadows::renderInMenu()
{
	TCompLightDir::renderInMenu();
	ImGui::Checkbox("Cast shadows", &generate_shadow);
}

void TCompLightDirShadows::start_editing() {
	if (original) delete original;
	original = new TCompLightDirShadows;
	*original = *this;
	original->original = false;
}
void TCompLightDirShadows::cancel_editing() {
	if (!original) return;
	TCompLightDirShadows * light_to_delete = original;
	*this = *original;
	if (light_to_delete) delete light_to_delete;
	if (original) delete original;
}

TCompLightDirShadows::~TCompLightDirShadows() {
	if (original) delete original;
}