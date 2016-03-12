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
#include "debug/debug.h"

#include <Commdlg.h>

bool CImGuiModule::start() {
	CApp& app = CApp::get();
	Debug = new CDebug();
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

	ImGui::Begin("Debug UI", &menu, ImVec2(800, 512), -1.0f, window_flags);
	ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels

	//Console log
	//---------------------------------------
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Console Debug"))
		{
			ImGui::MenuItem("Log", NULL, Debug->getStatus());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	//---------------------------------------

	//Buttons game
	//---------------------------------------
	ImGui::Button("PAUSE BUTTON - TODO");
	ImGui::SameLine();
	ImGui::Button("RESUME BUTTON - TODO");
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
	}

	ImGui::End();

	//TESTS DEBUG:
	//TestGameLog();
	//testLines();

	ui.update();			//update ui
	Debug->update();		//update log
}

void CImGuiModule::render() {
	//TODO: better way of deactive zbuffer?

	Render.ctx->OMSetRenderTargets(1, &Render.renderTargetView, NULL);
	ImGui::Render();
	Render.ctx->OMSetRenderTargets(1, &Render.renderTargetView, Render.zbuffer);
	//Debug->render();		//need to be called on game.cpp for now to get the technique and shader variables
	//TODO: fix that
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