#include "mcv_platform.h"
#include "sound_manager.h"
#include "app_modules/io/io.h"
#include "utils/utils.h"

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

bool CSoundManagerModule::playSound(std::string route) {
	Studio::EventInstance* sound_instance = NULL;
	result = sounds_descriptions[std::string(route)]->createInstance(&sound_instance);

	if (result == FMOD_OK) {
		sound_instance->start();
		sound_instance->release();
		return true;
	}

	return false;
}

bool CSoundManagerModule::playMusic(std::string route) {
	Studio::EventInstance* music_instance = NULL;
	result = sounds_descriptions[std::string(route)]->createInstance(&music_instance);

	if (result == FMOD_OK) {
		music_instance->start();
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


