#ifndef INC_COMPONENT_FADING_MESSAGE_H_
#define INC_COMPONENT_FADING_MESSAGE_H_

#include "comp_base.h"

class TCompFadingMessage : public TCompBase {
  std::string text;
  ImColor textColor;
  ImColor backgroudColor;
  float  ttl;
  float sizeFont = 0.025f;

public:
  //Update info

  void update(float elapsed);
  void render() const;
  bool load(MKeyValue& atts);
};

#endif
