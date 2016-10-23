#include "mcv_platform.h"
#include "editor_messages.h"
#include "handle/handle_manager.h"
#include "components/comp_fading_message.h"
#include <map>

using namespace MessageEditor;
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
	editing_text[0] = 0;
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
		auto name = all_sections[s].c_str();
		bool actived = section_chk[s];
		if (ImGui::Checkbox(name, &(actived)) && !changed_by_user) {
			changed_by_user = true;
			SetSection(s, actived);
		}
		if (cur_section == s) {
			RenderEntries();
		}
	}
	ImGui::Unindent();
}

void CEditorMessages::SetSection(int section, bool actived)
{
	if (actived) {
		for (int i = 0; i < section_chk.size(); i++) {
			section_chk[i] = (i == section);
		}
		cur_section = section;
		cur_entry = 0;
		cur_lang = 0;
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
	ImGui::InputTextMultiline("Edit", editing_text, 2048, ImVec2(500, 0));

	if (ImGui::Button("Show New")) {
		string editing_fixed = editing_text;
		editing_fixed = TextEncode::Utf8ToLatin1String(editing_fixed.c_str());
		ShowMessage(editing_fixed);
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
		auto name = ((*entries)[e]).c_str();
		if (ImGui::Checkbox(name, &actived)) {
			changed_by_user = true;
			SetEntry(e, actived);
		}
		if (cur_entry == e) {
			RenderLanguages();
		}
	}
	ImGui::Unindent();
}

void CEditorMessages::SetEntry(int entry, bool actived)
{
	if (actived) {
		auto entries = &all_entries[cur_section];
		for (int e = 0; e < entries->size(); e++) {
			entry_chk[cur_section][e] = (e == entry);
		}
		cur_entry = entry;
		cur_lang = 0;
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
			bool actived = lang_chk[l];
			if (ImGui::Checkbox(all_langs[l].c_str(), &actived)) {
				changed_by_user = true;
				SetLang(l, actived);
			}
		}
		else {
			ImGui::Text("%s (Not Available)", all_langs[l]);
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
		auto text = texts_by_lang[cur_lang][all_sections[cur_section]][all_entries[cur_section][cur_entry]];
		char in[2048];
		sprintf(in, text.c_str());
		unsigned char out[4096];
		unsigned char* in2 = (unsigned char*)in;
		unsigned char* out2 = out;
		while (*in2) {
			if ((*in2) < 128) *out2++ = *in2++;
			else *out2++ = 0xc2 + (*in2 > 0xbf), *out2++ = (*in2++ & 0x3f) + 0x80;
		}
		*out2 = 0;
		sprintf(original_text, "%s", out);
		sprintf(editing_text, "%s", out);
	}
	else {
		lang_chk[lang] = true; // No dejamos desactivar
	}
}