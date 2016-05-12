#ifndef INC_SOUND_MANAGER_H_
#define	INC_SOUND_MANAGER_H_

#include "app_modules/app_module.h"
#include "fmod.hpp"
#include "fmod_common.h"
#include "fmod_studio.h"
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string>

class CSoundManagerModule : public IAppModule
{
	std::string sounds_folder = "data/sounds";

	FMOD::System	 *system;
	std::map<std::string, FMOD::Sound*>		sounds;
	FMOD::Channel*							channels[4];
	FMOD_RESULT       result;
	unsigned int      version;
	void             *extradriverdata = 0;

public:

	// sound channels
	enum CHANNEL {
		SFX = 0,
		MUSIC,
		VOICES,
		AMBIENT
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
	bool playMusic(std::string);
	bool playVoice(std::string);
	bool playAmbient(std::string);
	void setVolume(CHANNEL, float);
	void stopChannel(CHANNEL channel);

};

extern CSoundManagerModule* sound_manager;

#endif
