#ifndef INC_COMPONENT_TRANSFORM_H_
#define INC_COMPONENT_TRANSFORM_H_

#include "geometry/transform.h"
#include "comp_msgs.h"
#include "comp_base.h"
class MKeyValue;

struct TCompTransform : public CTransform, public TCompBase {
  void render() const;
  bool load(MKeyValue& atts);
  void onCreate(const TMsgEntityCreated&) {
    dbg("TCompTransform on TMsgEntityCreated\n");
  }
  void renderInMenu();

  bool executeMovement(VEC3 new_pos);
};

#endif

