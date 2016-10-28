#include "mcv_platform.h"
#include "lang_manager.h"
#include "app_modules\io\io.h"
#include "utils\utils.h"
#include "components/entity.h"

#define PAD_SUFIX "_pad"
#define PAD_VERSION(entry) (entry + PAD_SUFIX)
#define ENTRY_NAME(entry) (entry.substr(2))

extern CLangManagerModule* lang_manager = nullptr;

CLangManagerModule::CLangManagerModule() {
	lang_file = lang_folder + "lang.json";
}
CLangManagerModule::CLangManagerModule(std::string language) {
	lang_file = lang_folder + "lang_" + language + ".json";
}

bool CLangManagerModule::start() {
	all_lang_sufixes.push_back("EN");
	all_lang_sufixes.push_back("ES");
	all_lang_sufixes.push_back("CAT");
	all_lang_sufixes.push_back("GA");

	auto file = CApp::get().file_options_json;
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
	BROADCAST_MSG(TMsgLanguageChanged);
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
	GetLanguageMap(language_map, lang_file);
	//auto scenes = CApp::get().GetNameScenes();
	////std::map<std::string, std::string> scenes = readIniAtrDataStr(CApp::get().file_options_json, "scenes");

	//typedef std::map<std::string, std::string>::iterator it_type;

	//// Read map texts
	//for (it_type iterator = scenes.begin(); iterator != scenes.end(); iterator++) {
	//	language_map[iterator->second] = readIniAtrDataStr(lang_file, iterator->second);
	//}

	//// Read action texts
	//language_map["actions"] = readIniAtrDataStr(lang_file, "actions");
}
void CLangManagerModule::GetLanguageMap(LangMap& lmap, std::string lang_file) {
	lmap.clear();
	auto scenes = CApp::get().GetNameScenes();
	//std::map<std::string, std::string> scenes = readIniAtrDataStr(CApp::get().file_options_json, "scenes");

	typedef std::map<std::string, std::string>::iterator it_type;

	// Read map texts
	for (it_type iterator = scenes.begin(); iterator != scenes.end(); iterator++) {
		lmap[iterator->second] = readIniAtrDataStr(lang_file, iterator->second);
	}

	// Read action texts
	lmap["actions"] = readIniAtrDataStr(lang_file, "actions");
	lmap["ui"] = readIniAtrDataStr(lang_file, "ui");
}

std::string CLangManagerModule::getText(std::string entry, std::string scene) {
	if (!HasEntryFormat(entry)) return entry;

	if (scene == "") scene = CApp::get().getCurrentRealLevel();
	auto scene_events = language_map[scene];

	if (io && io->IsGamePadMode() && isControllerMessage(entry, scene)) {
		entry = PAD_VERSION(entry);
	}

	entry = entry.substr(2);
	if (setContains(scene_events, entry)) {
		return scene_events[entry];
	}
	else return "";
}

bool CLangManagerModule::isControllerMessage(std::string entry, std::string scene) {
	if (entry.length() < 2 || entry[0] != ':' || entry[1] != ':') return false;

	if (scene == "") scene = CApp::get().getCurrentRealLevel();
	auto scene_events = language_map[scene];
	return setContains(scene_events, PAD_VERSION(ENTRY_NAME(entry)));
}

bool CLangManagerModule::HasEntryFormat(std::string entry)
{
	return entry.length() > 1 && entry[0] == ':' && entry[1] == ':';
}

void CLangManagerModule::GetAllTexts(MultiLangMap* all_texts) {
	all_texts->clear();
	for (int i = 0; i < all_lang_sufixes.size(); i++) {
		LangMap lmap;
		auto lang = all_lang_sufixes[i];
		std::string file = lang_folder + "lang_" + lang + ".json";

		auto scenes = CApp::get().GetNameScenes();
		//std::map<std::string, std::string> scenes = readIniAtrDataStr(CApp::get().file_options_json, "scenes");

		typedef std::map<std::string, std::string>::iterator it_type;

		// Read map texts
		for (it_type iterator = scenes.begin(); iterator != scenes.end(); iterator++) {
			lmap[iterator->second] = readIniAtrDataStr(file, iterator->second);
		}

		// Read action texts
		lmap["actions"] = readIniAtrDataStr(file, "actions");
		(*all_texts)[lang] = lmap;
	}
}

bool CLangManagerModule::ModifyEntry(std::string lang, std::string section, std::string entry, std::string new_text)
{
	//void writeIniAtrDataStr(const std::string route, std::string element, std::map<std::string, std::string> element_values)
	LangMap lmap;
	std::string route = lang_folder + "lang_" + lang + ".json";
	GetLanguageMap(lmap, route);
	lmap[section][entry] = new_text;
	writeIniAtrDataStr(route, section, lmap[section]);
	return true;
}