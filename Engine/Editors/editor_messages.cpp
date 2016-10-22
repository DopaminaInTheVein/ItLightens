#include "mcv_platform.h"
#include "editor_messages.h"

void CEditorMessages::update(float dt)
{
	if (controller->isEditorMessagesButtonPressed()) {
		m_activated_editor = !m_activated_editor;
	}
}

void CEditorMessages::LoadTexts()
{
}

void CEditorMessages::RenderInMenu()
{
	if (m_activated_editor) {
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(400, 700), ImGuiSetCond_Appearing);
		ImGui::Begin("Messages List", &m_activated_editor);
		//RenderGeneral();
		//RenderNewLight();
		//RenderAllLights();
		//RenderMultiEdit();
		ImGui::End();

		ImGui::SetNextWindowSize(ImVec2(512, 700), ImGuiSetCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(420, 0), ImGuiSetCond_Appearing);
		ImGui::Begin("Messages Editor", &m_activated_editor);
		//RenderGeneral();
		//RenderNewLight();
		//RenderAllLights();
		//RenderMultiEdit();
		ImGui::End();
	}
}