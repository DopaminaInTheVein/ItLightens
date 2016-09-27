#include "mcv_platform.h"
#include "comp_sound.h"

#include "logic/sbb.h"
#include "components/entity.h"
#include "components/comp_transform.h"
#include "components/comp_name.h"

#include "render/shader_cte.h"
#include "constants/ctes_camera.h"

extern CShaderCte< TCteCamera > shader_ctes_camera;

void TCompSound::init() {
	mParent = CHandle(this).getOwner();
	
	CEntity* entity = mParent;
	TCompTransform* transform = entity->get<TCompTransform>();
	entity_position = transform->getPosition();

	TCompName* name = entity->get<TCompName>();
	entity_name = name->name;

	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr_json;
	std::map<std::string, float> fields = readIniAtrData(file_ini, "sound");
	assignValueToVar(MAX_DISTANCE, fields);

	// create the sound
	sound_manager->playFixed3dSound(event, entity_name, entity_position, volume, true);
}

void TCompSound::update(float elapsed) {

	VEC3 camera_pos = shader_ctes_camera.CameraWorldPos;
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

	event = event;
	volume = volume;

	return true;
}

bool TCompSound::save(std::ofstream& os, MKeyValue& atts) {
	return true;
}

