#ifndef  INC_EDITOR_MESSAGES_H_
#define  INC_EDITOR_MESSAGES_H_

#define DEFAULT_LIGHTS ""

#include <vector>
#include "app_modules/lang_manager/lang_manager.h"

#define MAX_LANG 4
#define MAX_SECTIONS 10
#define MAX_ENTRIES 100

class CEditorMessages {
	bool m_activated_editor = false;
	MultiLangMap all_texts;
	std::vector<LangMap> texts_by_lang;
	std::vector<std::string> all_langs;
	std::vector<std::string> all_sections;
	std::vector<std::vector<std::string>> all_entries;
	std::vector<bool> lang_chk;
	std::vector<bool> lang_modif;
	int cur_lang = 0;
	std::vector<bool> section_chk;
	int cur_section = 0;
	std::vector<std::vector<bool>> entry_chk;
	int cur_entry = 0;
	bool keyboard = true;
	char original_text[2048] = "";
	char editing_text[MAX_LANG][MAX_SECTIONS][MAX_ENTRIES][255] = { "" };
	bool modified[MAX_LANG][MAX_SECTIONS][MAX_ENTRIES] = { false };
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
	void SaveFile();
	std::string GetOriginalText();
	bool CheckEntryModified(int s, int e);
	bool CheckSectionModified(int s);
	//char* GetEditingText();
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