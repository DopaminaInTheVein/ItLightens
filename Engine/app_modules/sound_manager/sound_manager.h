#ifndef INC_SOUND_MANAGER_H_
#define	INC_SOUND_MANAGER_H_

#include "app_modules/app_module.h"
#include "fmod.hpp"
#include "fmod_common.h"
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string>

class CSoundManagerModule : public IAppModule
{
	std::string sounds_folder = "data/sounds";

	FMOD::System	 *system;
	std::map<std::string, FMOD::Sound*>		sfx;
	FMOD::Channel    *channel = 0;
	FMOD_RESULT       result;
	unsigned int      version;
	void             *extradriverdata = 0;

public:

	CSoundManagerModule();
	bool start() override;
	void update(float dt) override;
	void stop() override;
	const char* getName() const {
		return "sound_manager";
	}

};

extern CSoundManagerModule* sound_manager;

#endif
