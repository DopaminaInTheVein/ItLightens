#ifndef INC_COMPONENT_CONTROLLER_1ST_PERSON_H_
#define INC_COMPONENT_CONTROLLER_1ST_PERSON_H_

#include "comp_base.h"
#include "handle/handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_msgs.h"
#include "geometry/angular.h"
#include "app_modules/io/io.h"

struct TCompController1stPerson : public TCompBase {
  float   rotation_sensibility;
  float   pan_sensibility;
  float   speed_reduction_factor;
  VEC3    speed;
  TCompController1stPerson()
    : rotation_sensibility(deg2rad(45.0f) / 50.0f)
    , pan_sensibility(0.1f)
    , speed(0.f,0.f,0.f)
    , speed_reduction_factor(0.99f)
  {} 

  void update(float dt) {

    CEntity* e_owner = CHandle(this).getOwner();
    TCompTransform* my_tmx = e_owner->get<TCompTransform>();
    VEC3 origin = my_tmx->getPosition();

    float yaw, pitch;
    my_tmx->getAngles(&yaw, &pitch);

    speed *= speed_reduction_factor;

    if (io->keys['W'].isPressed())
      speed.z += 1.f;
    if (io->keys['S'].isPressed())
      speed.z -= 1.f;
    if (io->keys['A'].isPressed())
      speed.x += 1.f;
    if (io->keys['D'].isPressed())
      speed.x -= 1.f;
    if (io->keys['Q'].isPressed())
      speed.y += 1.f;
    if (io->keys['E'].isPressed())
      speed.y -= 1.f;
    if (io->mouse.right.isPressed()) {
      speed.x -= io->mouse.dx;
      speed.y -= io->mouse.dy;
    } else {
      yaw -= io->mouse.dx * rotation_sensibility;
      pitch -= io->mouse.dy * rotation_sensibility;
    }

    if (io->mouse.wheel) {
      speed.z += io->mouse.wheel * 8;
    }
      
    origin += my_tmx->getLeft() * pan_sensibility * speed.x * dt;
    origin += my_tmx->getFront() * pan_sensibility * speed.z * dt;
    origin += my_tmx->getUp() * pan_sensibility * speed.y * dt;
    VEC3 front = getVectorFromYawPitch(yaw, pitch);
    
    my_tmx->lookAt(origin, origin + front);
  }

};

#endif

