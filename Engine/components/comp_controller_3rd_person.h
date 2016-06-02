#ifndef INC_COMPONENT_CONTROLLER_3RD_PERSON_H_
#define INC_COMPONENT_CONTROLLER_3RD_PERSON_H_

#include "comp_base.h"
#include "handle/handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_msgs.h"
#include "geometry/angular.h"
#include "app_modules/io/io.h"

struct TCompController3rdPerson : public TCompBase {
  CHandle target;
  float   yaw;
  float   pitch;
  float   distance_to_target;
  float   rotation_sensibility;
  TCompController3rdPerson()
  : yaw(deg2rad(0.f))
  , pitch(deg2rad(0.f))
  , distance_to_target( 5.0f )
  , rotation_sensibility( deg2rad( 90.0f ) / 25.0f )
  {} 

  void onSetTarget(const TMsgSetTarget& msg) {
    target = msg.target;
  }

  void update(float dt) {
    CEntity* e_target = target;
    if (!e_target)
      return;

    yaw += io->mouse.dx * rotation_sensibility;
    pitch += io->mouse.dy * rotation_sensibility;

    TCompTransform* target_tmx = e_target->get<TCompTransform>();
    assert(target_tmx);
    auto target_loc = target_tmx->getPosition();
    VEC3 delta = getVectorFromYawPitch(yaw, pitch);
    auto origin = target_loc - delta * distance_to_target;

    float distance_speed = 0.1f;
    if (io->keys['W'].isPressed())
      distance_to_target += distance_speed * dt;
    if (io->keys['S'].isPressed())
      distance_to_target -= distance_speed * dt;

    CEntity* e_owner = CHandle(this).getOwner();
    TCompTransform* my_tmx = e_owner->get<TCompTransform>();
    my_tmx->lookAt(origin, target_loc);
  }

};

#endif

