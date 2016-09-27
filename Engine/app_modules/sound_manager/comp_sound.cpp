#include "mcv_platform.h"
#include "comp_sound.h"

#include "logic/sbb.h"
#include "components/entity.h"
#include "components/comp_name.h"

#include "render/shader_cte.h"
#include "constants/ctes_camera.h"

extern CShaderCte< TCteCamera > shader_ctes_camera;

void TCompSound::init() {
	mParent = CHandle(this).getOwner();
	
	CEntity* entity = mParent;

	TCompName* name = entity->get<TCompName>();
	entity_name = name->name;

	hierarchy_comp = entity->get<TCompHierarchy>();
	if (hierarchy_comp) {
		TCompTransform* hierarchy_transform = hierarchy_comp->h_parent_transform;
		entity_position = hierarchy_transform->getPosition();
	}
	else {
		TCompTransform* transform = entity->get<TCompTransform>();
		entity_position = transform->getPosition();
	}

	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr_json;
	std::map<std::string, float> fields = readIniAtrData(file_ini, "sound");
	assignValueToVar(MAX_DISTANCE, fields);

	// create the sound
	sound_manager->playFixed3dSound("event:/" + event, entity_name, entity_position, volume, true);
}

void TCompSound::update(float elapsed) {

	VEC3 camera_pos = shader_ctes_camera.CameraWorldPos;

	// if the positions comes from the hierarchy parent, we have to update it
	if (hierarchy_comp) {
		TCompTransform* hierarchy_transform = hierarchy_comp->h_parent_transform;
		entity_position = hierarchy_transform->getPosition();
	}

	float dist = simpleDist(camera_pos, entity_position);

	// if we go out of distance, stop playing
	if (dist >= MAX_DISTANCE) {
		sound_manager->stopFixedSound(entity_name);
	}
	// otherwise, we update the sound
	else {
		sound_manager->updateFixed3dSound(entity_name, entity_position, volume);
	}
}

bool TCompSound::load(MKeyValue& atts) {

	event =  atts.getString("event", "OnUseGenerator");
	volume = atts.getFloat("volume", 0.25f);

	return true;
}

bool TCompSound::save(std::ofstream& os, MKeyValue& atts) {
	return true;
}

