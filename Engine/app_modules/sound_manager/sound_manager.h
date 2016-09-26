#ifndef INC_SOUND_MANAGER_H_
#define	INC_SOUND_MANAGER_H_

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string>
#include "app_modules/app_module.h"
#include "fmod_studio.hpp"
#include "fmod.hpp"

using namespace FMOD;

class CSoundManagerModule : public IAppModule
{
	std::string sounds_folder = "data/sounds/banks/";

	FMOD_RESULT												result;
	void													*extradriverdata = 0;

	System													*system = NULL; //low level system
	unsigned int											version;
	Studio::System											*studio_system = NULL;
	// Basic banks
	Studio::Bank											*masterBank = NULL;
	Studio::Bank											*stringsBank = NULL;
	// Specific banks
	Studio::Bank											*banks[4];
	// Sound descriptors
	Studio::EventDescription								**events_array[4];
	std::map<std::string, Studio::EventDescription*>		sounds_descriptions;
	// Global sound instances that need to be persistent
	Studio::EventInstance*									music_instance = NULL;
	std::map<std::string, Studio::EventInstance*>			fixed_instances;

public:

	// sound channels
	enum CHANNEL {
		SFX = 0,
		MUSIC,
		VOICES,
		AMBIENT,
		DUMMY
	};

	CSoundManagerModule();
	bool start() override;
	void update(float dt) override;
	void stop() override;
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "sound_manager";
	}

	bool playSound(std::string route , float volume, bool looping);
	bool play3dSound(std::string route, VEC3 sound_pos, float max_volume, bool looping, int max_instances);
	bool playFixed3dSound(std::string route, std::string sound_name, VEC3 sound_pos, float max_volume, bool looping);
	bool playMusic(std::string);
	bool playLoopingMusic(std::string);
	bool playVoice(std::string);
	bool playAmbient(std::string);

	bool stopSound(std::string route);
	bool stopFixedSound(std::string name);
	bool stopMusic();

	bool setMusicVolume(float);
	bool updateFixed3dSound(std::string name, VEC3 sound_pos, float max_volume);

	Studio::EventInstance* getFixedSound(std::string name) { return fixed_instances[name]; }
	FMOD_VECTOR VectorToFmod(const VEC3 vect);

};

extern CSoundManagerModule* sound_manager;

#endif
