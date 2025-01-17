#include "mcv_platform.h"
#include "editor_messages.h"
#include "handle/handle_manager.h"
#include "components/comp_fading_message.h"
#include <map>

#define EditingText (editing_text[cur_lang][cur_section][cur_entry])
#define TXT_MODIF " *** MODIFIED! ***"
using namespace std;
void CEditorMessages::update(float dt)
{
	if (controller->isEditorMessagesButtonPressed()) {
		m_activated_editor = !m_activated_editor;
	}
}

void CEditorMessages::LoadTexts()
{
	lang_manager->GetAllTexts(&all_texts);
	set<string> set_sections;
	set<pair<string, string>> set_entries;
	cur_lang = 0;
	cur_section = 0;
	cur_entry = 0;

	lang_chk.clear();
	texts_by_lang.clear();
	all_langs.clear();
	for (auto it_lang : all_texts) {
		all_langs.push_back(it_lang.first);
		texts_by_lang.push_back(it_lang.second);
		lang_chk.push_back(false);
		for (auto it_section : it_lang.second) {
			set_sections.insert(it_section.first);
			for (auto it_entry : it_section.second) {
				set_entries.insert(pair<string, string>(it_section.first, it_entry.first));
			}
		}
	}
	all_sections.clear();
	section_chk.clear();
	for (auto s : set_sections) {
		all_sections.push_back(s);
		section_chk.push_back(false);
	}
	all_entries.clear();
	all_entries.resize(all_sections.size());
	entry_chk.clear();
	entry_chk.resize(all_sections.size());
	for (auto e : set_entries) {
		auto pos = find(all_sections.begin(), all_sections.end(), e.first);
		all_entries[pos - all_sections.begin()].push_back(e.second);
		entry_chk[pos - all_sections.begin()].push_back(false);
	}

	//Init checks
	section_chk[0] = true;
	entry_chk[0][0] = true;
	lang_chk[0] = true;
}

void CEditorMessages::RenderInMenu()
{
	if (m_activated_editor) {
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(400, 700), ImGuiSetCond_Appearing);
		RenderMessageList();

		ImGui::SetNextWindowSize(ImVec2(512, 700), ImGuiSetCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(420, 0), ImGuiSetCond_Appearing);
		RenderEditor();
		changed_by_user = false;
	}
}

void CEditorMessages::RenderMessageList()
{
	ImGui::Begin("Messages List", &m_activated_editor);
	RenderSections();
	ImGui::End();
}
void CEditorMessages::RenderSections()
{
	ImGui::Text("SECTIONS");
	ImGui::Indent();
	for (int s = 0; s < all_sections.size(); s++) {
		string name = all_sections[s].c_str();
		bool actived = section_chk[s];
		if (CheckSectionModified(s)) name += TXT_MODIF;
		if (ImGui::Checkbox(name.c_str(), &(actived)) && !changed_by_user) {
			changed_by_user = true;
			SetSection(s, actived);
		}
		if (cur_section == s) {
			RenderEntries();
		}
	}
	ImGui::Unindent();
}

bool CEditorMessages::CheckSectionModified(int s)
{
	for (int e = 0; e < MAX_ENTRIES; e++) {
		if (CheckEntryModified(s, e)) return true;
	}
	return false;
}

void CEditorMessages::SetSection(int section, bool actived)
{
	if (actived) {
		for (int i = 0; i < section_chk.size(); i++) {
			section_chk[i] = (i == section);
		}
		cur_section = section;
		SetEntry(0, true);
	}
	else {
		section_chk[section] = true; // No dejamos desactivar
	}
}
void CEditorMessages::RenderEditor()
{
	ImGui::Begin("Messages Editor", &m_activated_editor);
	ImGui::Text("Section: %s", all_sections[cur_section].c_str());
	ImGui::Text("Entry: %s", all_entries[cur_section][cur_entry].c_str());
	ImGui::Text("Lang: %s", all_langs[cur_lang].c_str());
	ImGui::Separator();

	ImGui::Text("CURRENT TEXT");
	ImGui::InputTextMultiline("Original", original_text, 2048, ImVec2(500, 0), ImGuiInputTextFlags_ReadOnly);
	if (ImGui::Button("Show Original")) {
		string original_fixed = original_text;
		original_fixed = TextEncode::Utf8ToLatin1String(original_fixed.c_str());
		ShowMessage(original_fixed);
	}

	ImGui::Separator();

	ImGui::Text("EDIT TEXT");
	if (ImGui::InputTextMultiline("Edit", EditingText, 2048, ImVec2(500, 0))) {
		modified[cur_lang][cur_section][cur_entry] = (std::string(EditingText) != std::string(original_text));
	}

	if (ImGui::Button("Show New")) {
		string editing_fixed = EditingText;
		editing_fixed = TextEncode::Utf8ToLatin1String(editing_fixed.c_str());
		ShowMessage(editing_fixed);
	}
	if (ImGui::Button("Save to file")) {
		SaveFile();
	}
	ImGui::SameLine();
	if (ImGui::Button("Reload")) {
		sprintf(EditingText, "%s", original_text);
	}
	ImGui::End();
}

void CEditorMessages::ShowMessage(string text)
{
	getHandleManager<TCompFadingMessage>()->each([text](TCompFadingMessage * mess) {
		TCompFadingMessage::ReloadInfo atts;
		atts.text = text;
		mess->reload(atts);
	}
	);
}

void CEditorMessages::RenderEntries()
{
	ImGui::Indent();
	int section = cur_section;
	ImGui::Text("ENTRIES (%s)", all_sections[section].c_str());
	auto entries = &all_entries[section];
	for (int e = 0; e < entries->size(); e++) {
		bool actived = entry_chk[section][e];
		string name = ((*entries)[e]).c_str();
		if (CheckEntryModified(cur_section, e)) name += TXT_MODIF;
		if (ImGui::Checkbox(name.c_str(), &actived)) {
			changed_by_user = true;
			SetEntry(e, actived);
		}
		if (cur_entry == e) {
			RenderLanguages();
		}
	}
	ImGui::Unindent();
}
bool CEditorMessages::CheckEntryModified(int s, int e)
{
	for (int l = 0; l < MAX_LANG; l++) {
		if (modified[l][s][e]) return true;
	}
	return false;
}

void CEditorMessages::SetEntry(int entry, bool actived)
{
	if (actived) {
		auto entries = &all_entries[cur_section];
		for (int e = 0; e < entries->size(); e++) {
			entry_chk[cur_section][e] = (e == entry);
		}
		cur_entry = entry;
		SetLang(0, true);
	}
	else {
		entry_chk[cur_section][entry] = true; // No dejamos desactivar
	}
}

void CEditorMessages::RenderLanguages()
{
	ImGui::Indent();
	ImGui::Text("LANGUAGE");
	for (int l = 0; l < texts_by_lang.size(); l++) {
		if (CheckLanguage(l)) {
			bool available = CheckLanguage(l);
			bool actived = lang_chk[l];
			std::string lang_txt = all_langs[l];
			if (modified[l][cur_section][cur_entry]) lang_txt += TXT_MODIF;
			if (ImGui::Checkbox(lang_txt.c_str(), &actived)) {
				changed_by_user = true;
				SetLang(l, actived);
			}
		}
		else {
			ImGui::Text("%s (Not Available)", all_langs[l].c_str());
		}
	}
	ImGui::Unindent();
}

bool CEditorMessages::CheckLanguage(int lang)
{
	auto lmap = &texts_by_lang[lang];
	auto lsections = &((*lmap)[all_sections[cur_section]]);
	std::map<string, string> m = *lsections;
	std::string entry_to_search = all_entries[cur_section][cur_entry];
	return (lsections->find(entry_to_search) != lsections->end());
}

void CEditorMessages::SetLang(int lang, bool actived)
{
	if (actived) {
		for (int i = 0; i < lang_chk.size(); i++) {
			lang_chk[i] = (i == lang);
		}
		cur_lang = lang;
		auto text = GetOriginalText();
		text = TextEncode::Latin1ToUtf8String(text.c_str());
		sprintf(original_text, "%s", text.c_str());
		std::string cur_val(EditingText);
		if (cur_val == "") {
			sprintf(EditingText, "%s", text.c_str());
		}
	}
	else {
		lang_chk[lang] = true; // No dejamos desactivar
	}
}

std::string CEditorMessages::GetOriginalText()
{
	std::string res = "";
	auto lmap = texts_by_lang[cur_lang];
	if (lmap.find(all_sections[cur_section]) != lmap.end()) {
		auto smap = lmap[all_sections[cur_section]];
		if (smap.find(all_entries[cur_section][cur_entry]) != smap.end()) {
			res = smap[all_entries[cur_section][cur_entry]];
		}
	}
	return res;
}

void CEditorMessages::SaveFile()
{
	string editing_fixed = EditingText;
	editing_fixed = TextEncode::Utf8ToLatin1String(editing_fixed.c_str());
	lang_manager->ModifyEntry(all_langs[cur_lang], all_sections[cur_section], all_entries[cur_section][cur_entry], editing_fixed);
	sprintf(original_text, "%s", EditingText);
	all_texts[all_langs[cur_lang]][all_sections[cur_section]][all_entries[cur_section][cur_entry]] =
		texts_by_lang[cur_lang][all_sections[cur_section]][all_entries[cur_section][cur_entry]] = editing_fixed;
	modified[cur_lang][cur_section][cur_entry] = false;
}