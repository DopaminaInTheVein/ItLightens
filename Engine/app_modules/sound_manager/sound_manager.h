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
	// Global sound instances
	Studio::EventInstance*									music_instance = NULL;

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

	bool playSound(std::string);
	bool play3dSound(std::string, VEC3 player_pos, VEC3 sound_pos);
	bool stopSound(std::string);
	bool playMusic(std::string);
	bool playLoopingMusic(std::string);
	bool playVoice(std::string);
	bool playAmbient(std::string);
	bool setMusicVolume(float);

	//FMOD_RESULT F_CALLBACK markerCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters);

};

extern CSoundManagerModule* sound_manager;

#endif
