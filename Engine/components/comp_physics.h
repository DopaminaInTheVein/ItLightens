#ifndef INC_COMPONENT_PHYSICS_H_
#define INC_COMPONENT_PHYSICS_H_

#include "comp_base.h"
#include "comp_msgs.h"
// include physics.h
// ------------------------------------
struct TCompPhysics : public TCompBase {
  // physics id
  TCompPhysics() {
  }
  ~TCompPhysics() {
    // Destroy physics obj
  }
  // load Xml
  void onCreate(const TMsgEntityCreated&) {
    // Create obj in physics based on the configuration read from 
    // the xml
    // notify physics about the initial position 
    // asking my owner->get<TCompTransform>()
  }
  void updateFromPhysics() {
    // ask physics about the current pos + rotation
    // update my sibling TCompTransform with the physics info
  }
};

#endif

