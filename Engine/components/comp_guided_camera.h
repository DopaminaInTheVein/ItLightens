#ifndef INC_COMPONENT_GUIDED_CAMERA_H_
#define INC_COMPONENT_GUIDED_CAMERA_H_

#include "comp_base.h"

//Forward declaration
class TCompCamera;
class TCompTransform;

class TCompGuidedCamera : public TCompBase {
  std::vector<VEC3> points;
  int num_points;
  int num_cameras;
  float velocity;
  bool default_dirs;
  //float angularVelocity;
  std::vector<CQuaternion> rotations;
  std::vector<float> influences;
  std::vector<VEC3> cameraPositions;

  float maxInfluence = 0.0f;
  CQuaternion last_quat;
  int lastP = -1;

  int lastguidedCamPoint = 0;
  float factor = 0.0f;
public:
  TCompGuidedCamera() {
  }
  ~TCompGuidedCamera() {
  }

  bool load(MKeyValue& atts);
  void renderInMenu() {}
  void update(float dt) {}
  //int nearCameraPoint(VEC3 playerPosition);
  const int getTotalPoints() const { return num_points; }
  const float getVelocity() const { return velocity; }
  //  const float getAngularVelocity() const { return angularVelocity; }
  const VEC3 getCameraPosition(int posi) const { return cameraPositions[posi]; }
  const VEC3 getPointPosition(int posi) const { return points[posi]; }
  bool getDefaultDirsEnabled() { return default_dirs; }
  CQuaternion getNewRotationForCamera(VEC3 playerPosition, CQuaternion cameraActual, int pointOfInfluence);


  void onGuidedCamera(const TMsgGuidedCamera&);
  bool followGuide(TCompTransform*, TCompCamera*);
  void start();
};

#endif