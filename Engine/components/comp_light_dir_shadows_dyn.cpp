#include "mcv_platform.h"
#include "comp_light_dir_shadows_dyn.h"
#include "comp_transform.h"
#include "entity.h"
#include "render/draw_utils.h"
#include "resources/resources_manager.h"
#include "comp_room.h"
#include "player_controllers\player_controller_base.h"

void TCompLightDirShadowsDynamic::init() {
	PROFILE_FUNCTION("shadows: update");
	debug_render = true;
	CHandle owner = CHandle(this).getOwner();
	updateFromEntityTransform(owner);
}

void TCompLightDirShadowsDynamic::update(float dt) {
	PROFILE_FUNCTION("shadows: update");
	CHandle owner = CHandle(this).getOwner();
	CHandle player = CPlayerBase::handle_player;
	GET_COMP(t_player, player, TCompTransform);
	if (t_player) {
		VEC3 target = t_player->getPosition();
		if (last_position_target != target)
			this->lookAt(getPosition(), target);
	}
	//updateFromEntityTransform(owner);
}

void TCompLightDirShadowsDynamic::activate() {
	PROFILE_FUNCTION("shadows: activate");
	if (!enabled) return;
	CHandle owner = CHandle(this).getOwner();
	activateWorldMatrix(getViewProjection().Invert());
	rt_shadows->getZTexture()->activate(TEXTURE_SLOT_SHADOWMAP);
	uploadShaderCtes(owner);
}

bool TCompLightDirShadowsDynamic::save(std::ofstream& os, MKeyValue& atts) {
	TCompLightDir::save(os, atts);
	atts.put("resolution", res);
	return true;
}

void TCompLightDirShadowsDynamic::generateShadowMap() {
	if (!enabled)
		return;
	assert(rt_shadows);

	// Vamos a empezar a pintar en el shadow map
	rt_shadows->activateRT();
	rt_shadows->clearZ();
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

void TCompLightDirShadowsDynamic::destroy() {
	//if(rt_shadows)rt_shadows->destroy();
}

void TCompLightDirShadowsDynamic::setNewFov(float fov_in_rads) {
	float fov_in_degs = rad2deg(fov_in_rads);
	setProjection(fov_in_rads, getZNear(), getZFar());
}

void TCompLightDirShadowsDynamic::start_editing() {
	if (original) delete original;
	original = new TCompLightDirShadowsDynamic;
	*original = *this;
	original->original = false;
}
void TCompLightDirShadowsDynamic::cancel_editing() {
	if (!original) return;
	TCompLightDirShadowsDynamic * light_to_delete = original;
	*this = *original;
	if (light_to_delete) delete light_to_delete;
	if (original) delete original;
}

TCompLightDirShadowsDynamic::~TCompLightDirShadowsDynamic() {
	if (original) delete original;
}