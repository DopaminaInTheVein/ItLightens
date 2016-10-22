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

	

	return is_ok;
}
bool TCompLightDirShadows::save(std::ofstream& os, MKeyValue& atts) {
	TCompLightDir::save(os, atts);
	atts.put("resolution", res);
	return true;
}
void TCompLightDirShadows::update(float dt) {
	PROFILE_FUNCTION("shadows: update");
	CHandle owner = CHandle(this).getOwner();
	updateFromEntityTransform(owner);
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