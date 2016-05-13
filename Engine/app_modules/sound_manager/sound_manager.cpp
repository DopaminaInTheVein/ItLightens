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

	result = studio_system->getLowLevelSystem(&system);
	result = studio_system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extradriverdata);

	// basic banks
	result = studio_system->loadBankFile((sounds_folder + "Master Bank.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);
	result = studio_system->loadBankFile((sounds_folder + ".Master Bank.strings.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

	// specific banks
	result = studio_system->loadBankFile((sounds_folder + "Sfx.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[SFX]);
	result = studio_system->loadBankFile((sounds_folder + "Music.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[MUSIC]);
	result = studio_system->loadBankFile((sounds_folder + "Voices.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[VOICES]);
	result = studio_system->loadBankFile((sounds_folder + "Ambient.bank").c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &banks[AMBIENT]);

	//banks[SFX]->getEventList()

	/*
	// Cargamos los EventDescriptors
	Studio::EventDescription* explosionDescription = NULL;
	Studio::EventDescription* DaniDescription = NULL;
	res = system->getEvent("event:/Explosions/Single Explosion", &explosionDescription);
	res = system->getEvent("event:/Dani/OnEnter", &DaniDescription);

	// cacheo datos de la explosion para que no haya delays
	res = explosionDescription->loadSampleData();

	Studio::EventInstance* DaniInstance = NULL;
	res = DaniDescription->createInstance(&DaniInstance);


	bool end = false;

	while (!end)
	{
		char c = getch();
		if (c == 's')
		{
			// creo una explosión
			Studio::EventInstance* eventInstance = NULL;
			res = explosionDescription->createInstance(&eventInstance);
			eventInstance->start();
			// y la libero (esto se hará al acabar)
			eventInstance->release();
		}
		if (c == 'm')
		{
			// musiquilla
			res = DaniInstance->start();
		}
		if (c == 'd')
		{
			//musiquilla out
			res = DaniInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
		}
		if (c == 'q') end = true;
		system->update();
	}*/

	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
	if (result != FMOD_OK)
		return false;

	// load the scripts
	std::vector<std::string> files_to_load = list_files_recursively(sounds_folder);

	for (std::string file : files_to_load) {
		sounds[file] = nullptr;
		if (file.find("music") == std::string::npos && file.find("looping") == std::string::npos) {
			result = system->createSound(file.c_str(), FMOD_DEFAULT | FMOD_UNIQUE, 0, &sounds[file]);
		}
		else if (file.find("music") != std::string::npos) {
			// musics are loaded as unique streams and with loop mode on
			result = system->createStream(file.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL | FMOD_UNIQUE, 0, &sounds[file]);
		}
		else if (file.find("looping") != std::string::npos) {
			// looping sfx are looping but not unique
			result = system->createStream(file.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &sounds[file]);
		}

		if (result != FMOD_OK)
			return false;
	}

	// init channels
	channels[SFX] = 0;
	channels[MUSIC] = 0;
	channels[VOICES] = 0;
	channels[AMBIENT] = 0;

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

	stringsBank->unload();
	masterBank->unload();

	system->release();
}

bool CSoundManagerModule::playSound(std::string route) {
	result = system->playSound(sounds[route], 0, false, &channels[SFX]);

	return result == FMOD_OK;
}

bool CSoundManagerModule::playMusic(std::string route) {
	result = system->playSound(sounds[route], 0, false, &channels[MUSIC]);

	return result == FMOD_OK;
}

bool CSoundManagerModule::playVoice(std::string route) {
	result = system->playSound(sounds[route], 0, false, &channels[VOICES]);

	return result == FMOD_OK;
}

bool CSoundManagerModule::playAmbient(std::string route) {
	result = system->playSound(sounds[route], 0, false, &channels[AMBIENT]);

	return result == FMOD_OK;
}

void CSoundManagerModule::setVolume(CHANNEL channel, float volume) {
	// fit the volume to its min and max values
	if (volume < 0.f)
		volume = 0.f;
	else if (volume > 1.f)
		volume = 1.f;

	channels[channel]->setVolume(volume);
}

void CSoundManagerModule::stopChannel(CHANNEL channel) {
	// fit the volume to its min and max values

	channels[channel]->setPaused(true);
}

