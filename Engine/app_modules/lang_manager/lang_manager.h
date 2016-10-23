#ifndef INC_LANG_MANAGER_H_
#define	INC_LANG_MANAGER_H_

#include "app_modules/app_module.h"
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "SLB\SLB.hpp"

typedef std::map<std::string, std::map<std::string, std::string>> LangMap;
typedef std::map<std::string, LangMap> MultiLangMap;
class CLangManagerModule : public IAppModule
{
	std::string game_language = "EN";
	std::string lang_folder = "./data/lang/";
	std::string lang_file;
	std::vector<std::string> all_lang_sufixes;
	LangMap language_map;
	bool HasEntryFormat(std::string entry);
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

	std::string GetLanguage() const;
	void SetLanguage(std::string);

	void readLanguageMap();
	void reloadLanguageFile();
	std::string getText(std::string entry, std::string scene = "");
	bool isControllerMessage(std::string entry, std::string scene = "");
	static inline std::string toEntry(std::string text) { return "::" + text; }
	void GetAllTexts(MultiLangMap* all_texts);
	std::vector<std::string> GetAllLanguagesSufixes() { return all_lang_sufixes; }
};

extern CLangManagerModule* lang_manager;

#endif
