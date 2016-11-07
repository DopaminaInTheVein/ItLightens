#include "mcv_platform.h"
#include "sound_manager.h"
#include "utils/utils.h"

#include "render/shader_cte.h"
#include "constants/ctes_camera.h"

extern CShaderCte< TCteCamera > shader_ctes_camera;
extern CSoundManagerModule* sound_manager = nullptr;

CSoundManagerModule::CSoundManagerModule() {}

FMOD_RESULT F_CALLBACK loopingMusicCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters);
FMOD_RESULT F_CALLBACK loopingSoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters);
bool music_playing = true;

bool CSoundManagerModule::start() {

	/*CApp &app = CApp::get();
	std::string file_options_json = app.file_options_json;
	std::map<std::string, float> fields = readIniAtrData(file_options_json, "sound");
	assignValueToVar(MAX_DISTANCE, fields);*/

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

	result = Studio::System::create(&studio_system);
	result = studio_system->getLowLevelSystem(&system);
	result = studio_system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extradriverdata);

	// basic banks
	result = studio_system->loadBankFile((sounds_folder + "Master Bank.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);
	result = studio_system->loadBankFile((sounds_folder + "Master Bank.strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

	// specific banks
	result = studio_system->loadBankFile((sounds_folder + "Sfx.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[SFX]);
	result = studio_system->loadBankFile((sounds_folder + "Music.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[MUSIC]);
	result = studio_system->loadBankFile((sounds_folder + "Voices.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[VOICES]);
	result = studio_system->loadBankFile((sounds_folder + "Ambient.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[AMBIENT]);

	// get all events of every bank
	for (int channel = SFX; channel != DUMMY; channel++)
	{
		int eventCount = 0;
		Studio::Bank* bank = banks[channel];

		bank->getEventCount(&eventCount);
		Studio::EventDescription** array = (Studio::EventDescription**)malloc(eventCount * sizeof(void*));
		bank->getEventList(array, eventCount, &eventCount);

		for (int i = 0; i < eventCount; i++) {
			int maxLen = 255;
			char buffer[256];
			int retrieved;
			// get the path of the descriptor
			auto res = array[i]->getPath(buffer, maxLen, &retrieved);
			// store the descriptor in the map
			sounds_descriptions[std::string(buffer)] = NULL;
			result = studio_system->getEvent(buffer, &sounds_descriptions[std::string(buffer)]);

			// cacheo datos de la explosion para que no haya delays
			//result = sounds_descriptions[std::string(buffer)]->loadSampleData();
		}
	}

	return true;
}

void CSoundManagerModule::update(float dt) {
	studio_system->update();

	if (!music_playing)
		music_instance->start();
}

void CSoundManagerModule::stop() {
	// release sounds and system
	for (std::map<std::string, Studio::EventDescription*>::iterator sound_it = sounds_descriptions.begin(); sound_it != sounds_descriptions.end(); sound_it++) {
		sound_it->second->unloadSampleData();
	}
	sounds_descriptions.clear();

	// release sounds fixed instances and music
	for (std::map<std::string, Studio::EventInstance*>::iterator sound_it = fixed_instances.begin(); sound_it != fixed_instances.end(); sound_it++) {
		sound_it->second->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		sound_it->second->release();
	}
	fixed_instances.clear();

	music_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	music_instance->release();

	stringsBank->unload();
	masterBank->unload();

	for (int channel = SFX; channel != DUMMY; channel++)
		banks[channel]->unload();

	studio_system->release();
	system->release();
}

bool CSoundManagerModule::playSound(std::string route, float volume = 1.f, bool looping = false) {
	if (volume < 0.f) { volume = 0.f; }
	else if (volume > 1.f) { volume = 1.f; }

	Studio::EventInstance* sound_instance = NULL;

	int count;
	sounds_descriptions[std::string(route)]->getInstanceCount(&count);

	if (count < 1) {
		result = sounds_descriptions[std::string(route)]->createInstance(&sound_instance);

		if (result == FMOD_OK) {
			result = sound_instance->setVolume(volume*SFX_VOLUME);
			if (result != FMOD_OK) return false;

			if (looping) {
				result = sound_instance->setCallback(loopingSoundCallback, FMOD_STUDIO_EVENT_CALLBACK_STARTED | FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
				if (result != FMOD_OK) return false;

				result = sound_instance->start();
				if (result != FMOD_OK) return false;
			}
			else {
				result = sound_instance->start();
				if (result != FMOD_OK) return false;

				result = sound_instance->release();
				if (result != FMOD_OK) return false;
			}
			return true;
		}
	}

	return false;
}

bool CSoundManagerModule::play3dSound(std::string route, VEC3 sound_pos, float max_volume = 1.f, bool looping = false, int max_instances = 1) {
	Studio::EventInstance* sound_instance = NULL;

	int count;
	sounds_descriptions[std::string(route)]->getInstanceCount(&count);

	if (count < max_instances) {
		result = sounds_descriptions[std::string(route)]->createInstance(&sound_instance);

		if (result == FMOD_OK) {
			// normalize the maximum volume
			if (max_volume > 1.f) max_volume = 1.f;
			else if (max_volume < 0.f) max_volume = 0.f;

			// the volume will depend on the actual distance
			VEC3 camera_pos = shader_ctes_camera.CameraWorldPos;
			float dist = simpleDist(camera_pos, sound_pos);
			float volume = 1.f;

			if (dist < MAX_DISTANCE) {
				volume = (MAX_DISTANCE - dist) / MAX_DISTANCE;
				volume *= max_volume;
			}
			else {
				volume = 0.f;
			}

			camera_pos.Normalize();
			sound_pos.Normalize();
			VEC3 camera_front = shader_ctes_camera.CameraFront;
			VEC3 camera_up = shader_ctes_camera.CameraUp;

			FMOD_VECTOR listener_position = VectorToFmod(camera_pos);
			FMOD_VECTOR listener_front = VectorToFmod(camera_front);
			FMOD_VECTOR listener_up = VectorToFmod(-camera_up);
			FMOD_VECTOR sound_position = VectorToFmod(sound_pos);

			// Position the listener at the player position
			FMOD_3D_ATTRIBUTES attributes = { { 0 } };
			attributes.position = listener_position;
			attributes.forward = listener_front;
			attributes.up = listener_up;
			studio_system->setListenerAttributes(0, &attributes);

			// Position the event correctly
			attributes.position = sound_position;
			result = sound_instance->set3DAttributes(&attributes);
			if (result != FMOD_OK) return false;

			// Play the sound
			result = sound_instance->setVolume(volume*SFX_VOLUME);
			if (result != FMOD_OK) return false;

			if (looping) {
				result = sound_instance->setCallback(loopingSoundCallback, FMOD_STUDIO_EVENT_CALLBACK_STARTED | FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
				if (result != FMOD_OK) return false;

				result = sound_instance->start();
				if (result != FMOD_OK) return false;
			}
			else {
				result = sound_instance->start();
				if (result != FMOD_OK) return false;

				result = sound_instance->release();
				if (result != FMOD_OK) return false;
			}

			// Reset listener attributes
			attributes.position = { { 0 } };
			studio_system->setListenerAttributes(0, &attributes);
			return true;
		}
	}

	return false;
}

bool CSoundManagerModule::playFixed3dSound(std::string route, std::string sound_name, VEC3 sound_pos, float max_volume, bool looping) {
	// the sound is not created yet
	if (fixed_instances[sound_name] == NULL) {
		result = sounds_descriptions[std::string(route)]->createInstance(&fixed_instances[sound_name]);

		if (result == FMOD_OK) {
			// normalize the maximum volume
			if (max_volume > 1.f) max_volume = 1.f;
			else if (max_volume < 0.f) max_volume = 0.f;

			// the volume will depend on the actual distance
			VEC3 camera_pos = shader_ctes_camera.CameraWorldPos;
			float dist = simpleDist(camera_pos, sound_pos);
			float volume = 1.f;

			if (dist < MAX_DISTANCE) {
				volume = (MAX_DISTANCE - dist) / MAX_DISTANCE;
				volume *= max_volume;
			}
			else {
				volume = 0.f;
			}

			camera_pos.Normalize();
			sound_pos.Normalize();
			VEC3 camera_front = shader_ctes_camera.CameraFront;
			VEC3 camera_up = shader_ctes_camera.CameraUp;

			FMOD_VECTOR listener_position = VectorToFmod(camera_pos);
			FMOD_VECTOR listener_front = VectorToFmod(camera_front);
			FMOD_VECTOR listener_up = VectorToFmod(-camera_up);
			FMOD_VECTOR sound_position = VectorToFmod(sound_pos);

			// Position the listener at the player position
			FMOD_3D_ATTRIBUTES attributes = { { 0 } };
			attributes.position = listener_position;
			attributes.forward = listener_front;
			attributes.up = listener_up;
			studio_system->setListenerAttributes(0, &attributes);

			// Position the event correctly
			attributes.position = sound_position;
			result = fixed_instances[sound_name]->set3DAttributes(&attributes);
			if (result != FMOD_OK) return false;

			// Play the sound
			result = fixed_instances[sound_name]->setVolume(volume*SFX_VOLUME);
			if (result != FMOD_OK) return false;

			if (looping) {
				result = fixed_instances[sound_name]->setCallback(loopingSoundCallback, FMOD_STUDIO_EVENT_CALLBACK_STARTED | FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
				if (result != FMOD_OK) return false;

				result = fixed_instances[sound_name]->start();
				if (result != FMOD_OK) return false;
			}
			else {
				result = fixed_instances[sound_name]->start();
				if (result != FMOD_OK) return false;

				result = fixed_instances[sound_name]->release();
				if (result != FMOD_OK) return false;
			}

			// Reset listener attributes
			attributes.position = { { 0 } };
			studio_system->setListenerAttributes(0, &attributes);
			return true;
		}
	}
	return false;
}

bool CSoundManagerModule::stopSound(std::string route) {
	int count;
	sounds_descriptions[std::string(route)]->getInstanceCount(&count);

	Studio::EventInstance** sound_instances = (Studio::EventInstance**)malloc(count * sizeof(void*));
	sounds_descriptions[std::string(route)]->getInstanceList(sound_instances, count, &count);

	if (count > 0) {
		for (int i = 0; i < count; i++) {
			Studio::EventInstance* sound_instance = NULL;
			sound_instance = sound_instances[i];

			result = sound_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			if (result != FMOD_OK) return false;

			result = sound_instance->release();
			if (result != FMOD_OK) return false;
		}
		return true;
	}

	return false;
}

bool CSoundManagerModule::stopFixedSound(std::string name) {
	PROFILE_FUNCTION("stopFixedSound");
	result = fixed_instances[name]->setPaused(true);
	return result == FMOD_OK;
}

bool CSoundManagerModule::destroyFixed3dSound(std::string name) {
	PROFILE_FUNCTION("destroyFixed3dSound");
	result = fixed_instances[name]->setPaused(true);
	if (result != FMOD_OK) return false;
	result = fixed_instances[name]->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	if (result != FMOD_OK) return false;
	result = fixed_instances[name]->release();
	return result == FMOD_OK;
}

bool CSoundManagerModule::stopAllSounds() {
	// we iterate over all the sound descriptors and stop and release each instance
	for (std::map<std::string, Studio::EventDescription*>::iterator it = sounds_descriptions.begin(); it != sounds_descriptions.end(); ++it) {
		int count;
		it->second->getInstanceCount(&count);

		Studio::EventInstance** sound_instances = (Studio::EventInstance**)malloc(count * sizeof(void*));
		it->second->getInstanceList(sound_instances, count, &count);

		if (count > 0) {
			for (int i = 0; i < count; i++) {
				Studio::EventInstance* sound_instance = NULL;
				sound_instance = sound_instances[i];

				result = sound_instance->setPaused(true);
				if (result != FMOD_OK) return false;

				result = sound_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
				if (result != FMOD_OK) return false;

				result = sound_instance->release();
				if (result != FMOD_OK) return false;
			}
		}
	}

	fixed_instances.clear();

	return true;
}

bool CSoundManagerModule::updateFixed3dSound(std::string sound_name, VEC3 sound_pos, float max_volume) {
	// if the sound was paused, we resume it
	PROFILE_FUNCTION("updateFixedSound");
	bool paused;
	result = fixed_instances[sound_name]->getPaused(&paused);
	if (result != FMOD_OK) return false;

	if (paused) {
		result = fixed_instances[sound_name]->setPaused(false);
		if (result != FMOD_OK) return false;
		result = fixed_instances[sound_name]->start();
		if (result != FMOD_OK) return false;
	}

	// normalize the maximum volume
	if (max_volume > 1.f) max_volume = 1.f;
	else if (max_volume < 0.f) max_volume = 0.f;

	// update the volume
	VEC3 camera_pos = shader_ctes_camera.CameraWorldPos;
	float dist = simpleDist(camera_pos, sound_pos);
	float volume = 1.f;

	if (dist < MAX_DISTANCE) {
		volume = (MAX_DISTANCE - dist) / MAX_DISTANCE;
		volume *= max_volume;
	}
	else {
		volume = 0.f;
	}

	result = fixed_instances[sound_name]->setVolume(volume*SFX_VOLUME);
	if (result != FMOD_OK) return false;

	// update the position
	camera_pos.Normalize();
	sound_pos.Normalize();
	VEC3 camera_front = shader_ctes_camera.CameraFront;
	VEC3 camera_up = shader_ctes_camera.CameraUp;

	FMOD_VECTOR listener_position = VectorToFmod(camera_pos);
	FMOD_VECTOR listener_front = VectorToFmod(camera_front);
	FMOD_VECTOR listener_up = VectorToFmod(-camera_up);
	FMOD_VECTOR sound_position = VectorToFmod(sound_pos);

	FMOD_3D_ATTRIBUTES attributes = { { 0 } };
	attributes.position = listener_position;
	attributes.forward = listener_front;
	attributes.up = listener_up;
	studio_system->setListenerAttributes(0, &attributes);

	attributes.position = sound_position;
	result = fixed_instances[sound_name]->set3DAttributes(&attributes);
	if (result != FMOD_OK) return false;

	// Reset listener attributes
	attributes.position = { { 0 } };
	studio_system->setListenerAttributes(0, &attributes);
	return true;
}

bool CSoundManagerModule::playMusic(std::string route, float volume = 0.3f) {
	//if there was a music playing, we pause it
	if (music_instance) {
		music_instance->setPaused(true);
	}

	//if the music instance didnt exist yet, we create it
	if (!fixed_instances[route]) {
		result = sounds_descriptions[std::string(route)]->createInstance(&fixed_instances[route]);
		if (result != FMOD_OK) return false;
		result = fixed_instances[route]->setCallback(loopingMusicCallback, FMOD_STUDIO_EVENT_CALLBACK_STARTED | FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
		if (result != FMOD_OK) return false;
		//set the music to the requested track, and play it
		music_instance = fixed_instances[route];
		music_instance->setVolume(volume);
		result = music_instance->start();
		if (result != FMOD_OK) return false;
	}
	// if the music already existed, we set the music to the instance and unpause it
	else {
		//set the music to the requested track, and play it
		music_instance = fixed_instances[route];
		music_instance->setVolume(volume);
		result = music_instance->setPaused(false);
		if (result != FMOD_OK) return false;
	}

	return true;
}

bool CSoundManagerModule::stopMusic() {
	//if there was a music playing, we pause it
	if (music_instance) {
		result = music_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		if (result != FMOD_OK) return false;
		result = music_instance->release();
		if (result != FMOD_OK) return false;
		music_instance = nullptr;
	}

	return true;
}

bool CSoundManagerModule::playVoice(std::string route) {
	Studio::EventInstance* voice_instance = NULL;
	result = sounds_descriptions[std::string(route)]->createInstance(&voice_instance);

	if (result == FMOD_OK) {
		voice_instance->start();
		voice_instance->release();
		return true;
	}

	return false;
}

bool CSoundManagerModule::playAmbient(std::string route) {
	Studio::EventInstance* ambient_instance = NULL;
	result = sounds_descriptions[std::string(route)]->createInstance(&ambient_instance);

	if (result == FMOD_OK) {
		ambient_instance->start();
		ambient_instance->release();
		return true;
	}

	return false;
}

bool CSoundManagerModule::setMusicVolume(float volume) {
	if (volume < 0.f) { volume = 0.f; }
	if (volume > 1.f) { volume = 1.f; }

	result = music_instance->setVolume(volume);
	if (result == FMOD_OK)
		return true;

	return false;
}

void CSoundManagerModule::setSFXVolume(float volume) {
	if (volume < 0.f) { volume = 0.f; }
	if (volume > 1.f) { volume = 1.f; }

	SFX_VOLUME = volume;
}

FMOD_VECTOR CSoundManagerModule::VectorToFmod(const VEC3 vect) {
	FMOD_VECTOR fVec;
	fVec.x = vect.x;
	fVec.y = vect.y;
	fVec.z = vect.z;
	return fVec;
}

// Callback from Studio - Remember these callbacks will occur in the Studio update thread, NOT the game thread.
FMOD_RESULT F_CALLBACK loopingMusicCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters)
{
	if (type == FMOD_STUDIO_EVENT_CALLBACK_STARTED)
	{
		music_playing = true;
	}
	else if (type == FMOD_STUDIO_EVENT_CALLBACK_STOPPED)
	{
		music_playing = false;
	}
	else {
		//Nothing
	}

	return FMOD_OK;
}

// Callback from Studio - Remember these callbacks will occur in the Studio update thread, NOT the game thread.
FMOD_RESULT F_CALLBACK loopingSoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters)
{
	FMOD::Studio::EventInstance* instance = (FMOD::Studio::EventInstance*)event;

	if (type == FMOD_STUDIO_EVENT_CALLBACK_STOPPED)
	{
		instance->setCallback(loopingSoundCallback, FMOD_STUDIO_EVENT_CALLBACK_STARTED | FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
		instance->start();
	}

	return FMOD_OK;
}