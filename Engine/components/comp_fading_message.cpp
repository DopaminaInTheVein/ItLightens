#include "mcv_platform.h"
#include "comp_fading_message.h"
#include "entity.h"
#include "app_modules/gui/gui_utils.h"
#include "imgui/imgui_internal.h"
#include "resources/resources_manager.h"
#include "render/render.h"
#include "render/DDSTextureLoader.h"

bool TCompFadingMessage::load(MKeyValue& atts)
{
  text = atts.getString("text", "defaultText");
  //ttl = atts.getFloat("ttl", 0.1f);
  ttl = timeForLetter * text.length() + 2.0f;
  std::string textColorStr = atts.getString("textColor", "#FFFFFFFF");
  std::string backgroudColorStr = atts.getString("backgroundColor", "#000000FF");
  textColor = obtainColorFromString(textColorStr);
  backgroudColor = obtainColorFromString(backgroudColorStr);
  iconUri = "icons/" + atts.getString("icon", "none.dds");
  iconLittleText = atts.getString("iconText", "DEFAULT");
  numchars = 0;

  lines = 1;
  std::string endline = "\n";
  size_t pos = text.find(endline, 0);
  while (pos != text.npos)
  {
    lines++;
    pos = text.find(endline, pos + 1);
  }
  if (lines < minlines) {
    lines = minlines;
  }
  resolution_x = CApp::get().getXRes();
  resolution_y = CApp::get().getYRes();
  flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
  marginForImage = lines * percentLineHeight;
  startxrect = 0.0f;
  startyrect = 0.95f - marginForImage;
  textureIcon = /*const_cast<CTexture *>*/(Resources.get(iconUri.c_str())->as<CTexture>());
  if (!textureIcon->isValid()) {
    iconLoaded = false;
  }
  else {
    iconLoaded = true;
    tex_id = (ImTextureID*)textureIcon->getResView();
  }

  return true;
}

void TCompFadingMessage::update(float dt) {
  static float accumTime = 0.0f;

  accumTime += getDeltaTime();
  while (accumTime > timeForLetter) {
    ++numchars;
    accumTime -= timeForLetter;
  }

  if (ttl >= 0.0f) {
    ttl -= getDeltaTime();
  }
  else {
    iconLoaded = false;
    //textureIcon->destroy();
    CHandle h = CHandle(this).getOwner();
    h.destroy();
  }
}
void TCompFadingMessage::render() const {
  // ttl message is viewed
  std::string textToShow = text.substr(0, numchars);

  bool b = false;

  ImGui::Begin("Game GUI", &b, ImVec2(resolution_x, resolution_y), 0.0f, flags);
  ImGui::SetWindowSize("Game GUI", ImVec2(resolution_x, resolution_y));

  Rect rect = GUI::createRect(startxrect, startyrect, 1.0f, 1.0f);
  GUI::drawRect(rect, backgroudColor);
  GUI::drawText(startxrect + percentLineHeight + percentLineHeight + marginForImage, startyrect + percentLineHeight, GImGui->Font, sizeFont, textColor, textToShow.c_str());
  if (iconLoaded) {
    GUI::drawImage(startxrect + percentLineHeight, startyrect + percentLineHeight, startxrect + percentLineHeight + marginForImage, startyrect + percentLineHeight + marginForImage, tex_id);
  }
  else {
    GUI::drawText(startxrect + marginForImage / 2, startyrect + marginForImage / 2, GImGui->Font, sizeFont, textColor, iconLittleText.c_str());
  }
  ImGui::End();
}