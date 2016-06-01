#ifndef INC_GEOMETRY_TRANSFORM_H_
#define INC_GEOMETRY_TRANSFORM_H_

#include "geometry.h"

// ---------------------------
class CTransform {
  CQuaternion rotation;
  VEC3        position;
  VEC3        scale;
public:
  // ---------------------------
  CTransform( ) : scale( 1.f, 1.f, 1.f ) {}
  MAT44       asMatrix() const {
    return MAT44::CreateScale(scale) 
      * MAT44::CreateFromQuaternion(rotation)
      * MAT44::CreateTranslation(position);
  }

  CQuaternion getRotation() const { return rotation; }
  VEC3        getPosition() const { return position; }
  VEC3        getScale() const { return scale; }

  // ---------------------------
  // Yes, these are not super efficient
  VEC3        getFront() const {
    MAT44 mrot = MAT44::CreateFromQuaternion(rotation);
    return mrot.Backward();
  }
  VEC3        getLeft() const {
    // To be coherent with our local system
    MAT44 mrot = MAT44::CreateFromQuaternion(rotation);
    return mrot.Right();
  }
  VEC3        getUp() const {
    // To be coherent with our local system
    MAT44 mrot = MAT44::CreateFromQuaternion(rotation);
    return mrot.Up();
  }

  // ---------------------------
  void       getAngles( float* yaw, float* pitch, float* roll = nullptr ) const {
    VEC3 f = getFront();
    getYawPitchFromVector(f, yaw, pitch);

    // If requested...
    if (roll) {
      VEC3 roll_zero_left = VEC3(0, 1, 0).Cross(getFront());
      VEC3 roll_zero_up = getFront().Cross(roll_zero_left);
      *roll = atan2(getLeft().Dot(roll_zero_up), getLeft().Dot(roll_zero_left));
    }

  }
  void setAngles(float new_yaw, float new_pitch, float new_roll = 0.f) {
    rotation = CQuaternion::CreateFromYawPitchRoll(new_yaw, -new_pitch, new_roll);
  }

  float getDeltaYawToAimTo(VEC3 target) const;
  bool isHalfConeVision(VEC3 target, float half_cone_in_rads) const;
  bool isInFront(VEC3 target) const;
  bool isInLeft(VEC3 target) const;

  // ---------------------------
  void        setPosition(VEC3 new_pos) {
    position = new_pos;
  }
  void        setScale(VEC3 new_scale) {
    scale = new_scale;
  }
  void        setRotation(CQuaternion new_rotation) {
    rotation = new_rotation;
    rotation.Normalize();
  }

  void        lookAt(VEC3 eye, VEC3 target, VEC3 up = VEC3(0,1,0)) {
    // As the CreateLookAt is used for cameras, which have the +Z
    // to the -front, we use this same function but tweak the params
    // so the +Z is looking to target
    auto delta = target - eye;
    auto m = MAT44::CreateLookAt(eye, eye - delta, up).Invert();
    m.Decompose(scale, rotation, position);
  }

  void combine(const CTransform& parent, const CTransform& delta) {
    assert(&delta != this);
    setPosition(parent.getPosition() + VEC3::Transform(delta.getPosition(), parent.rotation));
    setRotation(delta.getRotation() * parent.getRotation());
    setScale(parent.getScale() * delta.getScale());
  }

};



#endif

