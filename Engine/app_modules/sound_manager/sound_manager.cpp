#include "mcv_platform.h"
#include "sound_manager.h"
#include "app_modules\io\io.h"

#include "utils\utils.h"

extern CSoundManagerModule* sound_manager = nullptr;

CSoundManagerModule::CSoundManagerModule() {}

bool CSoundManagerModule::start() {

	/*
	Create a System object and initialize
	*/
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK)
		return false;

	result = system->getVersion(&version);
	if (result != FMOD_OK)
		return false;

	if (version < FMOD_VERSION)
	{
		dbg("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
		return false;
	}

	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
	if (result != FMOD_OK)
		return false;

	// load the scripts
	std::vector<std::string> files_to_load = list_files_recursively(sounds_folder);

	for (std::string file : files_to_load) {
		sounds[file] = nullptr;
		if (file.find("music") == std::string::npos) {
			result = system->createSound(file.c_str(), FMOD_DEFAULT, 0, &sounds[file]);
		}
		else {
			result = system->createStream(file.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &sounds[file]);
		}
		if (result != FMOD_OK)
			return false;
	}

	return true;
}

void CSoundManagerModule::update(float dt) {

}

void CSoundManagerModule::stop() {
	// release sounds and system
	for (std::map<std::string, FMOD::Sound*>::iterator sound_it = sounds.begin(); sound_it != sounds.end(); sound_it++) {
		sound_it->second->release();
	}
	sounds.clear();
	system->release();
}

bool CSoundManagerModule::playSound(std::string route) {
	result = system->playSound(sounds[route], 0, false, &channel);

	return result == FMOD_OK;
}
