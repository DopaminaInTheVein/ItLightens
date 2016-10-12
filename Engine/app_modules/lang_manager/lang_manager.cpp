#include "mcv_platform.h"
#include "lang_manager.h"
#include "app_modules\io\io.h"
#include "utils\utils.h"
#include "components/entity.h"

extern CLangManagerModule* lang_manager = nullptr;

CLangManagerModule::CLangManagerModule() {
	lang_file = lang_folder + "lang.json";
}
CLangManagerModule::CLangManagerModule(std::string language) {
	lang_file = lang_folder + "lang_" + language + ".json";
}

bool CLangManagerModule::start() {
	auto file = CApp::get().get().file_options_json;
	auto language = readIniAtrDataStr(file, "language");
	game_language = language["lang"];

	readLanguageMap();
	return true;
}

std::string CLangManagerModule::GetLanguage() const
{
	return game_language;
}
void CLangManagerModule::SetLanguage(std::string lang)
{
	if (lang == game_language) return;
	game_language = lang;
	lang_manager->reloadLanguageFile();
	getHandleManager<CEntity>()->each([](CEntity * e) {
		e->sendMsg(TMsgLanguageChanged());
	});
}

void CLangManagerModule::reloadLanguageFile() {
	lang_file = lang_folder + "lang_" + game_language + ".json";
	readLanguageMap();
}

void CLangManagerModule::update(float dt) {
	// do nothing
}

void CLangManagerModule::stop() {
	language_map.clear();
}

void CLangManagerModule::readLanguageMap() {
	auto scenes = CApp::get().GetNameScenes();
	//std::map<std::string, std::string> scenes = readIniAtrDataStr(CApp::get().file_options_json, "scenes");

	typedef std::map<std::string, std::string>::iterator it_type;

	// Read map texts
	for (it_type iterator = scenes.begin(); iterator != scenes.end(); iterator++) {
		language_map[iterator->second] = readIniAtrDataStr(lang_file, iterator->second);
	}

	// Read action texts
	language_map["actions"] = readIniAtrDataStr(lang_file, "actions");
}

std::string CLangManagerModule::getText(std::string scene, std::string entry) {
	auto scene_events = language_map[scene];
	if (io && io->IsGamePadMode()) {
		std::string entry_pad = entry + "_pad";
		if (setContains(scene_events, entry_pad)) {
			return scene_events[entry_pad];
		}
	}
	if (setContains(scene_events, entry)) {
		return scene_events[entry];
	}
	else return "";
}