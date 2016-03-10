#ifndef INC_COMPONENT_NAME_H_
#define INC_COMPONENT_NAME_H_

#include "utils/XMLParser.h"
#include "comp_base.h"
#include "imgui/imgui.h"

struct TCompName : public TCompBase {
  static const size_t max_name_length = 64;
  char name[max_name_length];
  bool load(MKeyValue& atts) {
    strcpy(name, atts.getString("name", "no_name").c_str());
    return true;
  }
  void renderInMenu() {
    ImGui::InputText("Name", name, max_name_length-1 );
  }
};

#endif

