#include "mcv_platform.h"
#include "geometry.h"

float CTransform::getDeltaYawToAimTo(VEC3 target) const {

  VEC3 me_to_target = target - getPosition();
  float parallel_part = me_to_target.Dot(getFront());
  VEC3 left = getLeft();
  float perp_part = me_to_target.Dot(getLeft());
  float angle = atan2f(perp_part, parallel_part);
  return angle;

}

bool CTransform::isHalfConeVision(VEC3 target, float half_cone_in_rads) const {
  return fabsf( getDeltaYawToAimTo(target) ) <= half_cone_in_rads;
}

bool CTransform::isInFront(VEC3 target) const {
  return fabsf(getDeltaYawToAimTo(target)) <= deg2rad( 90.f );
}

bool CTransform::isInLeft(VEC3 target) const {
  return getDeltaYawToAimTo(target) > 0.f;
}


