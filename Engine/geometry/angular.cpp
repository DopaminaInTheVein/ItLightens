#include "mcv_platform.h"

void getYawPitchFromVector(const VEC3& front, float* yaw, float *pitch) {
  *yaw = atan2(front.x, front.z);
  float mdo = sqrtf(front.x*front.x + front.z*front.z);
  *pitch = atan2(front.y, mdo);
}

VEC3 getVectorFromYawPitch(float yaw, float pitch) {
  return VEC3(
      sin(yaw) * cos(pitch)
    , sin(pitch)
    , cos(yaw) * cos(pitch)
    );
}


