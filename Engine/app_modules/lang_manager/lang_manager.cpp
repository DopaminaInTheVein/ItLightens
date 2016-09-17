#include "mcv_platform.h"
#include "lang_manager.h"
#include "app_modules\io\io.h"
#include "utils\utils.h"

extern CLangManagerModule* lang_manager = nullptr;

CLangManagerModule::CLangManagerModule() {
	lang_file = lang_folder + "lang.json";
}
CLangManagerModule::CLangManagerModule(std::string language) {
	lang_file = lang_folder + "lang_" + language + ".json";
}

bool CLangManagerModule::start() {	
	readLanguageMap();
	return true;
}

void CLangManagerModule::reloadLanguageFile(std::string language) {
	lang_file = lang_folder + "lang_" + language + ".json";
	readLanguageMap();
}

void CLangManagerModule::update(float dt) {
	// do nothing
}

void CLangManagerModule::stop() {
	language_map.clear();
}

void CLangManagerModule::readLanguageMap() {

	std::map<std::string, std::string> scenes = readIniAtrDataStr(CApp::get().file_options_json, "scenes");

	typedef std::map<std::string, std::string>::iterator it_type;

	// Read map texts
	for (it_type iterator = scenes.begin(); iterator != scenes.end(); iterator++) {
		language_map[iterator->second] = readIniAtrDataStr(lang_file, iterator->second);
	}

	// Read action texts
	language_map["actions"] = readIniAtrDataStr(lang_file, "actions");
}

std::string CLangManagerModule::getText(std::string scene, std::string event) {
	return language_map[scene][event];
}

