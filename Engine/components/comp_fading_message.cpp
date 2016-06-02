#include "mcv_platform.h"
#include "comp_fading_message.h"
#include "entity.h"
#include "app_modules\gui\gui_utils.h"
#include "imgui\imgui_internal.h"

bool TCompFadingMessage::load(MKeyValue& atts)
{
  text = atts.getString("text", "defaultText");
  ttl = atts.getFloat("ttl", 0.1f);
  std::string textColorStr = atts.getString("textColor", "#FFFFFFFF");
  std::string backgroudColorStr = atts.getString("backgroundColor", "#000000FF");
  textColor = obtainColorFromString(textColorStr);
  backgroudColor = obtainColorFromString(backgroudColorStr);
  return true;
}

void TCompFadingMessage::update(float dt) {
  if (ttl >= 0.0f) {
    ttl -= getDeltaTime();
  }
  else {
    CHandle h = CHandle(this).getOwner();
    h.destroy();
  }
}
void TCompFadingMessage::render() const {
  // ttl message is viewed
  int lines = 1;
  float percentLineHeight = 0.02f;
  std::string endline = "\n";
  size_t pos = text.find(endline, 0);
  while (pos != text.npos)
  {
    lines++;
    pos = text.find(endline, pos + 1);
  }

  float resolution_x = CApp::get().getXRes();
  float resolution_y = CApp::get().getYRes();
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings;
  bool b = false;

  ImGui::Begin("Game GUI", &b, ImVec2(resolution_x, resolution_y), 0.0f, flags);
  ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));

  float startxrect = 0.0f;
  float startyrect = 0.95f - lines*percentLineHeight;

  Rect rect = GUI::createRect(startxrect, startyrect, 1.0f, 1.0f);
  GUI::drawRect(rect, backgroudColor);
  GUI::drawText(startxrect + percentLineHeight, startyrect + percentLineHeight, GImGui->Font, sizeFont, textColor, text.c_str());
  ImGui::End();
}