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
#ifdef PROFILING_ENABLED
    static int nframes = 5;
    ImGui::InputInt("NFrames", &nframes, 1, 5);
    if (ImGui::Button("Start Capture Profiling"))
      profiler.setNFramesToCapture(nframes);
#endif
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  }

}

void CImGuiModule::render() {
  ImGui::Render();
}
 
bool CImGuiModule::onSysMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  return ImGui_ImplDX11_WndProcHandler(hWnd, message, wParam, lParam) ? true : false;
}


