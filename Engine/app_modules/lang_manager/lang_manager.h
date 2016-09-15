#ifndef INC_LANG_MANAGER_H_
#define	INC_LANG_MANAGER_H_

#include "app_modules/app_module.h"
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "SLB\SLB.hpp"

class CLangManagerModule : public IAppModule
{
	std::string lang_folder = "./data/lang/";
	std::string lang_file;

	std::map<std::string, std::map<std::string, std::string>> language_map;

public:

	CLangManagerModule();
	CLangManagerModule(std::string language);
	bool start() override;
	void update(float dt) override;
	void stop() override;
	bool forcedUpdate() { return true; }
	const char* getName() const {
		return "lang_manager";
	}

	void readLanguageMap();
	void reloadLanguageFile(std::string language);
	std::string getText(std::string scene, std::string event);

};

extern CLangManagerModule* lang_manager;

#endif
