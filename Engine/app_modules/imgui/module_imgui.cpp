#include "mcv_platform.h"
#include "module_imgui.h"
#include "windows/app.h"
#include "resources/resources_manager.h"
#include "app_modules/entities.h"
#include "handle/handle_manager.h"
#include "components/entity_tags.h"
// ImGui LIB headers
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui.h"
#pragma comment(lib, "imgui.lib" )

#include "handle/object_manager.h"
#include "render/draw_utils.h"

#include <Commdlg.h>

bool CImGuiModule::start() {
	CApp& app = CApp::get();
	return ImGui_ImplDX11_Init(app.getHWnd(), Render.device, Render.ctx);
}

void CImGuiModule::stop() {
	ImGui_ImplDX11_Shutdown();
}

void CImGuiModule::update(float dt) {
	ImGui_ImplDX11_NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	bool menu = true;
#ifndef NDEBUG
	ImGui::Begin("Debug UI", &menu, ImVec2(800, 512), -1.0f, window_flags);
	ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

	//Console log
	//---------------------------------------
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Console Debug"))
		{
			ImGui::MenuItem("Log (L)", NULL, Debug->getStatus());
			ImGui::MenuItem("Commands (O)", NULL, Debug->GetCommandsConsoleState());
			//Debug->OpenConsole();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	//---------------------------------------

	//Buttons game
	//---------------------------------------
	if (GameController->GetGameState() == CGameController::RUNNING) {
		if (ImGui::Button("PAUSE BUTTON"))
			GameController->SetGameState(CGameController::STOPPED);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
		if (ImGui::Button("RESUME BUTTON"))
			GameController->SetGameState(CGameController::RUNNING);

		ImGui::PopStyleColor();
	}

	if (GameController->GetGameState() == CGameController::STOPPED) {
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 1, 0, 1));
		if (ImGui::Button("PAUSE BUTTON"))
			GameController->SetGameState(CGameController::STOPPED);

		ImGui::PopStyleColor();

		ImGui::SameLine();

		if (ImGui::Button("RESUME BUTTON"))
			GameController->SetGameState(CGameController::RUNNING);
	}

	ImGui::Checkbox("Free camera (K)", GameController->GetFreeCameraPointer());
	//ImGui::Checkbox("Continous Collision Detection", &(g_PhysxManager->ccdActive));

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
	ImGui::Text("WARNING: The player will still move, pause the game to stop moving the player");
	ImGui::PopStyleColor();

	if (ImGui::TreeNode("Free camera instructions")) {
		ImGui::Text("w/a/s/d - normal move\nq/e - up/down\nmouse wheel - speed up/down\n");

		ImGui::TreePop();
	}

	ImGui::Separator();
	//---------------------------------------

	//Profiling
	//---------------------------------------
#ifdef PROFILING_ENABLED
  //header for filtering instructions
	if (ImGui::TreeNode("PROFILING"))
	{
		static int nframes = 5;
		ImGui::InputInt("NFrames", &nframes, 1, 5);
		if (ImGui::Button("Start Capture Profiling"))
			profiler.setNFramesToCapture(nframes);
		ImGui::TreePop();
	}
#endif
	//---------------------------------------

	//Filter options
	//---------------------------------------
	ImGui::Text("Filtering");

	//header for filtering instructions
	if (ImGui::TreeNode("instructions filtering"))
	{
		ImGui::Text("Filter usage:\n"
			"  \"\"         display all lines\n"
			"  \"xxx\"      display lines containing \"xxx\"\n"
			"  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
			"  \"-xxx\"     hide lines containing \"xxx\"");
		ImGui::TreePop();
	}
	//end header instructions

	static ImGuiTextFilter filter;
	filter.Draw(); //filter text draw
	//---------------------------------------

	if (ImGui::CollapsingHeader("General Info")) {
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	if (ImGui::CollapsingHeader("Resources")) {
		Resources.renderUIDebug(&filter);
	}

	if (ImGui::CollapsingHeader("Entities")) {
		getHandleManager<CEntity>()->onAll(&CEntity::renderInMenu);
	}

	if (ImGui::CollapsingHeader("SELECTED ENTITY")) {
		ImGui::Text("Application SELECTED ENTITY - TODO");
	}if (ImGui::CollapsingHeader("Entity by Tag")) {
		ImGui::Text("Application ENTITY TAG - TODO");
		tags_manager.renderInMenu();
	}if (ImGui::CollapsingHeader("Graficos")) {
		if (ImGui::TreeNode("polarize")) {
			ImGui::SliderFloat("Polarize strength", &shader_ctes_globals.strenght_polarize, 0.0f, 2.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Grafic options")) {
			ImGui::Checkbox("polarize effects", GameController->GetFxPolarizePointer());

			ImGui::Checkbox("glow effect", GameController->GetFxGlowPointer());

			ImGui::TreePop();
		}
	}if (ImGui::CollapsingHeader("Culling")) {
		RenderManager.renderUICulling();
		ImGui::Checkbox("show culling collider", GameController->GetCullingRenderPointer());
	}

	ImGui::End();
#endif
	//TESTS DEBUG:
	//TestGameLog();
	//testLines();
	//bool open = true;
	//ShowExampleAppConsole(&open);		//test console commands

	ui.update();			//update ui
	//Debug->update();		//update log
}

void CImGuiModule::render() {
	activateZ(ZCFG_ALL_DISABLED);
	ImGui::Render();
}

bool CImGuiModule::onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam) ? true : false;
}

//Open file path
std::string CImGuiModule::getFilePath(char * filter, HWND owner)
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	std::string fileNameStr;
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}