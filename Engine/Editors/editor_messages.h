#ifndef  INC_EDITOR_MESSAGES_H_
#define  INC_EDITOR_MESSAGES_H_

#define DEFAULT_LIGHTS ""

#include <vector>
#include "app_modules/lang_manager/lang_manager.h"

namespace MessageEditor {
	//class VSections;
	//class Section;
	//class TextEntry;
	//struct LangEntry {
	//	std::string name;
	//	std::string text;
	//	bool keyboard;
	//	bool gamepad;
	//	TextEntry* parent = nullptr;
	//};

	//class Section;
	//struct TextEntry {
	//	std::string name;
	//	std::map<std::map<std::string, std::string>, LangEntry> langs;
	//	LangEntry* cur_lang;
	//	Section* parent = nullptr;
	//	TextEntry(std::string lang, std::string new_name, std::string text);
	//};
	//struct Section {
	//	std::string name;
	//	std::map<std::map<std::string, std::string>, TextEntry> entries;
	//	TextEntry* cur_entry = nullptr;
	//	Section(std::string lang, std::string name, std::map<std::string, std::string>);
	//};
	//struct VSection {
	//	MultiLangMap all_texts;
	//	std::map<std::map<std::string, std::string>, Section> sections;
	//	Section* cur_section = nullptr;
	//	void reload();
	//};
};

class CEditorMessages {
	bool m_activated_editor = false;
	MultiLangMap all_texts;
	std::vector<LangMap> texts_by_lang;
	std::vector<std::string> all_langs;
	std::vector<std::string> all_sections;
	std::vector<std::vector<std::string>> all_entries;
	std::vector<bool> lang_chk;
	int cur_lang = 0;
	std::vector<bool> section_chk;
	int cur_section = 0;
	std::vector<std::vector<bool>> entry_chk;
	int cur_entry = 0;
	bool keyboard = true;
	bool gamepad;
	char original_text[2048] = "";
	char editing_text[2048] = "";
	bool changed_by_user = false;

	void RenderMessageList();
	void RenderSections();
	void RenderEntries();
	void RenderLanguages();
	void RenderEditor();

	void SetSection(int section, bool actived);
	void SetEntry(int entry, bool actived);
	void SetLang(int lang, bool actived);
	bool CheckLanguage(int lang);
	void ShowMessage(std::string text);
public:
	bool* GetEditorState() { return &m_activated_editor; }
	void SetEditorState(bool value) {
		m_activated_editor = value;
	}
	void ToggleEditorState() {
		m_activated_editor = !m_activated_editor;
	}
	void LoadTexts();
	void update(float dt);
	void RenderInMenu();
};
#endif