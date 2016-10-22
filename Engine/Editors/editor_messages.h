#ifndef  INC_EDITOR_MESSAGES_H_
#define  INC_EDITOR_MESSAGES_H_

#define DEFAULT_LIGHTS ""

#include <vector>

class CEditorMessages {
	bool m_activated_editor = false;
public:
	bool* GetEditorState() { return &m_activated_editor; }
	void SetEditorState(bool value) {
		m_activated_editor = value;
	}
	void ToggleEditorState() {
		m_activated_editor = !m_activated_editor;
	}
	void RenderInMenu();
	void LoadTexts();
	void update(float dt);
};

#endif