#ifndef	INC_CONSOLE_H_
#define INC_CONSOLE_H_

#include "imgui/imgui.h"
#include "utils/utils.h"
#include "input/input_wrapper.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// Some compilers support applying printf-style warnings to user functions.
#if defined(__clang__) || defined(__GNUC__)
#define IM_PRINTFARGS(FMT) __attribute__((format(printf, FMT, (FMT+1))))
#else
#define IM_PRINTFARGS(FMT)
#endif

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.
class CConsole
{
	char                  InputBuf[256];
	ImVector<char*>       Items;
	bool                  ScrollToBottom;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImVector<const char*> Commands;
	bool				  opened = false;

	//TODO: should be able to call lua functions
	//functions commands done:
	void Command_Help(const char* command) {
		AddLog("Commands:");
		for (int i = 0; i < Commands.Size; i++)
			AddLog("- %s", Commands[i]);
	}

	void Command_Test(std::vector<char*> &args) {
		AddLog("test function");

		int i = 1;
		for (char* a : args) {
			AddLog("#arg %d - %s\n", i, a);
			i++;
		}
	}

	void Command_TeleportXYZ(std::vector<char*>& args);

	void Command_Teleport(std::vector<char*> &args);
	void Command_Lua(std::vector<char*> &args);

	//Paste
	//Clipboard text
	std::string GetClipboardText()
	{
		// Try opening the clipboard
		if (!OpenClipboard(nullptr)) return "";

		// Get handle of clipboard object for ANSI text
		HANDLE hData = GetClipboardData(CF_TEXT);
		if (hData == nullptr) return "";

		// Lock the handle to get the actual text pointer
		char * pszText = static_cast<char*>(GlobalLock(hData));
		if (pszText == nullptr) return "";

		// Save text in a string class instance
		std::string text(pszText);

		// Release the lock
		GlobalUnlock(hData);

		// Release the clipboard
		CloseClipboard();

		return text;
	}

public:

	CConsole()
	{
		ClearLog();
		memset(InputBuf, 0, sizeof(InputBuf));
		HistoryPos = -1;
		LoadCommands();
	}
	~CConsole()
	{
		ClearLog();
		for (int i = 0; i < Items.Size; i++)
			free(History[i]);
	}

	void SetOpened(bool new_opened) { opened = new_opened; }
	bool * GetOpened() { return &opened; }

	void LoadCommands() {
		//commands list goes here
		Commands.push_back("CLEAR");
		Commands.push_back("HELP");
		Commands.push_back("TEST_CONSOLE");
		Commands.push_back("TELEPORT");
	}

	void    ClearLog()
	{
		for (int i = 0; i < Items.Size; i++)
			free(Items[i]);
		Items.clear();
		ScrollToBottom = true;
	}

	void    AddLog(const char* fmt, ...) IM_PRINTFARGS(2)
	{
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		Items.push_back(_strdup(buf));
		ScrollToBottom = true;
	}

	void    Draw(const char* title, bool* opened)
	{
		ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiSetCond_FirstUseEver);
		if (!ImGui::Begin(title, opened))
		{
			ImGui::End();
			return;
		}

		ImGui::TextWrapped("Enter 'HELP' for help, press TAB to use text completion.\nPress up/down to move between used commands");

		if (ImGui::SmallButton("Clear")) ClearLog();

		ImGui::Separator();

		// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
		// NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
		// If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear")) ClearLog();
			ImGui::EndPopup();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		for (int i = 0; i < Items.Size; i++)
		{
			const char* item = Items[i];
			ImVec4 col = ImColor(255, 255, 255); // A better implementation may store a type per-item. For the sample let's just parse the text.
			if (strstr(item, "[error]")) col = ImColor(255, 100, 100);
			else if (strncmp(item, "# ", 2) == 0) col = ImColor(255, 200, 150);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::TextUnformatted(item);
			ImGui::PopStyleColor();
		}
		if (ScrollToBottom)
			ImGui::SetScrollHere();

		ScrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		if (!controller->isReleaseButtonPressed()) {
			//ImGui::SetWindowFocus();

			// Command-line
			if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
			{
				char* input_end = InputBuf + strlen(InputBuf);
				while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
				if (InputBuf[0])
					ExecCommand(InputBuf);
				strcpy(InputBuf, "");
			}

			// Demonstrate keeping auto focus on the input box
			//(ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
			/*if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive()))
				ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget*/
			if (ImGui::Button("Paste")) {
				std::string clipboard = GetClipboardText();
				strcpy(InputBuf, clipboard.c_str());
			}
		}
		ImGui::End();
	}

	//Compare text, without mayus or minus
	static int Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
	static int Strnicmp(const char* str1, const char* str2, int count) { int d = 0; while (count > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; count--; } return d; }

	void    ExecCommand(const char* command_line)
	{
		AddLog("# %s\n", command_line);

		// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
		HistoryPos = -1;
		for (int i = History.Size - 1; i >= 0; i--)
			if (Stricmp(History[i], command_line) == 0)
			{
				free(History[i]);
				History.erase(History.begin() + i);
				break;
			}
		History.push_back(_strdup(command_line));

		char* line_to_treat = _strdup(command_line);

		//split command by "-"
		command_line = strtok(line_to_treat, " ");		//command name
		//command_line = line_to_treat;

		//char * command_args = strtok(NULL, "-");		//list args

		//iterate all args
		char * command_args = strtok(NULL, " ");
		std::vector<char *> args;
		while (command_args != NULL) {
			args.push_back(command_args);
			command_args = strtok(NULL, " ");
		}

		//clean whitespace from command name
		char* space_end = strchr(line_to_treat, ' ');
		space_end = '\0';
		command_line = line_to_treat;

		// Process command
		if (Stricmp(command_line, "CLEAR") == 0)
		{
			ClearLog();
		}
		else if (Stricmp(command_line, "HELP") == 0)
		{
			Command_Help(command_line);
		}
		else if (Stricmp(command_line, "TEST_CONSOLE") == 0) {
			Command_Test(args);
		}
		else if (Stricmp(command_line, "TELEPORT") == 0) {
			Command_Teleport(args);
		}
		else if (Stricmp(command_line, "LUA") == 0) {
			Command_Lua(args);
		}
		else {
			AddLog("[error]Unknown command: '%s'\n", command_line);
		}

		free(space_end);
		free(line_to_treat);
		free(command_args);
		args.clear();
	}

	static int TextEditCallbackStub(ImGuiTextEditCallbackData* data) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
	{
		CConsole* console = (CConsole*)data->UserData;
		return console->TextEditCallback(data);
	}

	int     TextEditCallback(ImGuiTextEditCallbackData* data)
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION

			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf)
			{
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				word_start--;
			}

			// Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < Commands.Size; i++)
				if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
					candidates.push_back(Commands[i]);

			if (candidates.Size == 0)
			{
				// No match
				AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
			}
			else if (candidates.Size == 1)
			{
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
				int match_len = (int)(word_end - word_start);
				for (;;)
				{
					int c = 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0)
							c = toupper(candidates[i][match_len]);
						else if (c != toupper(candidates[i][match_len]))
							all_candidates_matches = false;
					if (!all_candidates_matches)
						break;
					match_len++;
				}

				if (match_len > 0)
				{
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
				}

				// List matches
				AddLog("Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++)
					AddLog("- %s\n", candidates[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
					HistoryPos = History.Size - 1;
				else if (HistoryPos > 0)
					HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size)
						HistoryPos = -1;
			}

			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				snprintf(data->Buf, data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
				data->BufDirty = true;
				data->CursorPos = data->SelectionStart = data->SelectionEnd = (int)strlen(data->Buf);
			}
		}
		}
		return 0;
	}

	void update() {
		if (opened) {
			if (controller->IsBackPressed()) {
				opened = false;
				return;
			}
			Draw("Commands", &opened);
		}
	}
};

//testing function
static void ShowExampleAppConsole(bool* opened)
{
	static CConsole console;
	console.Draw("Console commands", opened);
}

#endif