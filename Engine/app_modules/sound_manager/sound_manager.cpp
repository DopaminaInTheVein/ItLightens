#include "mcv_platform.h"
#include "sound_manager.h"
#include "utils/utils.h"

extern CSoundManagerModule* sound_manager = nullptr;

CSoundManagerModule::CSoundManagerModule() {}

FMOD_RESULT F_CALLBACK markerCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters);
bool music_playing = true;

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

	stringsBank->unload();
	masterBank->unload();

	for (int channel = SFX; channel != DUMMY; channel++)
		banks[channel]->unload();

	studio_system->release();
	system->release();
}

bool CSoundManagerModule::playSound(std::string route, float volume = 1.f) {

	if (volume < 0.f) { volume = 0.f; }
	else if (volume > 1.f) { volume = 1.f; }

	Studio::EventInstance* sound_instance = NULL;

	int count;
	sounds_descriptions[std::string(route)]->getInstanceCount(&count);

	if (count < 1) {
		result = sounds_descriptions[std::string(route)]->createInstance(&sound_instance);

		if (result == FMOD_OK) {
			sound_instance->setVolume(volume);
			sound_instance->start();
			sound_instance->release();
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
			sound_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			sound_instance->release();
		}
		return true;		
	}

	return false;
}

bool CSoundManagerModule::play3dSound(std::string route, VEC3 player_pos, VEC3 sound_pos, float volume = 1.f) {

	if (volume < 0.f) { volume = 0.f; }
	else if (volume > 1.f) { volume = 1.f; }

	Studio::EventInstance* sound_instance = NULL;

	int count;
	sounds_descriptions[std::string(route)]->getInstanceCount(&count);

	if (count < 1) {
		result = sounds_descriptions[std::string(route)]->createInstance(&sound_instance);
		if (result == FMOD_OK) {

			// Position the listener at the player position
			FMOD_3D_ATTRIBUTES attributes = { { 0 } };
			attributes.position.x = player_pos.x;
			attributes.position.y = player_pos.y;
			attributes.position.z = player_pos.z;
			attributes.forward.z = 1.0f;
			attributes.up.y = -1.0f;
			studio_system->setListenerAttributes(0, &attributes);

			// Position the event correctly
			attributes.position.x = sound_pos.x;
			attributes.position.y = sound_pos.y;
			attributes.position.z = sound_pos.z;
			sound_instance->set3DAttributes(&attributes);

			// Play the sound
			sound_instance->setVolume(volume);
			sound_instance->start();
			sound_instance->release();

			// Reset listener attributes
			attributes.position.x = 0;
			attributes.position.y = 0;
			attributes.position.z = 0;
			studio_system->setListenerAttributes(0, &attributes);
			return true;
		}
	}

	return false;
}

bool CSoundManagerModule::playMusic(std::string route) {

	//if there was a music playing, we stop it
	if (music_instance) {
		music_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		music_instance->release();
		music_instance = NULL;
	}

	result = sounds_descriptions[std::string(route)]->createInstance(&music_instance);

	music_instance->start();

	if (result == FMOD_OK) {
		music_instance->start();
		return true;
	}

	return false;
}

bool CSoundManagerModule::playLoopingMusic(std::string route) {

	//if there was a music playing, we stop it
	if (music_instance) {
		music_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		music_instance->release();
		music_instance = NULL;
	}

	result = sounds_descriptions[std::string(route)]->createInstance(&music_instance);

	if (result == FMOD_OK) {
		music_instance->setCallback(markerCallback, FMOD_STUDIO_EVENT_CALLBACK_STARTED | FMOD_STUDIO_EVENT_CALLBACK_STOPPED);
		music_instance->start();
		return true;
	}

	return false;

}

bool CSoundManagerModule::stopMusic() {

	//if there was a music playing, we stop it
	if (music_instance) {
		music_instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		music_instance->release();
		music_instance = NULL;
		return true;
	}

	return false;
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

// Callback from Studio - Remember these callbacks will occur in the Studio update thread, NOT the game thread.
FMOD_RESULT F_CALLBACK markerCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters)
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

