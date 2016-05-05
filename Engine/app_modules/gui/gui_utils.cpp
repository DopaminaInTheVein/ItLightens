#include "mcv_platform.h"
#include "gui_utils.h"

void GUI::drawRect(Rect rect, ImColor color) {
  ImGui::GetWindowDrawList()->AddRectFilled(
    ImVec2(rect.x, rect.y),
    ImVec2(rect.x + rect.sx, rect.y + rect.sy),
    color
    );
}

void GUI::drawText(float x, float y, ImFont * font, float size, ImColor color, const char * text) {
  int res_x = CApp::get().getXRes();
  int res_y = CApp::get().getYRes();
  Pixel p(x * res_x, y * res_y);
  drawText(p, font, size, color, text);
}

void GUI::drawText(Pixel p, ImFont * font, float size, ImColor color, const char * text) {
  float sizeFont = size * CApp::get().getYRes();

  ImGui::GetWindowDrawList()->AddText(
    font,
    sizeFont,
    ImVec2(p.x, p.y),
    color,
    text
    );
}

Rect GUI::createRect(float x, float y, float sx, float sy) {
  int res_x = CApp::get().getXRes();
  int res_y = CApp::get().getYRes();
  return Rect(x* res_x, y*res_y, sx* res_x, sy*res_y);
}