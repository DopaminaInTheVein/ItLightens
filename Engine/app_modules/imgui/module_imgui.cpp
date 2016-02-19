#include "mcv_platform.h"
#include "module_imgui.h"
#include "windows/app.h"

// ImGui LIB headers
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui.h"
#pragma comment(lib, "imgui.lib" )

bool CImGuiModule::start() {
  CApp& app = CApp::get();
  return ImGui_ImplDX11_Init(app.getHWnd(), Render.device, Render.ctx);
}

void CImGuiModule::stop() {
  ImGui_ImplDX11_Shutdown();
}

void CImGuiModule::update(float dt) {
  ImGui_ImplDX11_NewFrame();

  // 1. Show a simple window
  // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
  {
    static bool show_another_window = false;
    static ImVec4 clear_col = ImColor(114, 144, 154);
    static float f = 0.0f;
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&clear_col);
    if (ImGui::Button("Another Window")) show_another_window ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (show_another_window)
    {
      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello");
      ImGui::End();
    }
  }

}

void CImGuiModule::render() {
  ImGui::Render();
}
 
bool CImGuiModule::onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  return ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam) ? true : false;
}


