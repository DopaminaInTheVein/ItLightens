#ifndef INC_COMPONENT_FADING_MESSAGE_H_
#define INC_COMPONENT_FADING_MESSAGE_H_

#include "comp_base.h"

class TCompFadingMessage : public TCompBase {
  float percentLineHeight = 0.02f;
  float timeForLetter = 0.05f;
  float marginForImage;
  int numchars;
  int lines;
  int minlines = 3;
  float resolution_x;
  float resolution_y;
  ImGuiWindowFlags flags;
  float startxrect;
  float startyrect;

  std::string text;
  ImColor textColor;
  ImColor backgroudColor;
  float  ttl;
  float sizeFont = 0.025f;
  std::string iconUri;
  const CTexture * textureIcon;
  std::string iconLittleText;
  bool iconLoaded;
  ImTextureID tex_id;

public:
  //Update info

  void update(float elapsed);
  void render() const;
  bool load(MKeyValue& atts);
};

#endif
