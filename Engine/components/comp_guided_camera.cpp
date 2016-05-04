#include "mcv_platform.h"
#include "comp_guided_camera.h"
#include "comp_transform.h"
#include "entity.h"

#define WPT_ATR_NAME(nameVariable, index) \
char nameVariable[10]; sprintf(nameVariable, "wpt%d", index)
#define ROT_ATR_NAME(nameVariable, index) \
char nameVariable[10]; sprintf(nameVariable, "rot%d", index)

bool TCompGuidedCamera::load(MKeyValue& atts) {
  num_points = atts.getInt("points_size", 0);
  velocity = atts.getFloat("velocity", 0);
  angularVelocity = atts.getFloat("angularVelocity", 0);
  num_cameras = num_points - 1;
  points.resize(num_points);
  rotations.resize(num_cameras);
  influences.resize(num_cameras);
  cameraPositions.resize(num_cameras);
  for (int i = 0; i < num_points; i++) {
    WPT_ATR_NAME(atrPos, i);
    points[i] = atts.getPoint(atrPos);
  }
  for (int i = 0; i < num_cameras; i++) {
    ROT_ATR_NAME(atrRot, i);
    CQuaternion rot = atts.getQuat(atrRot);
    rot.Normalize();
    rotations[i] = rot;
    influences[i] = realDist(points[i], points[i + 1]) / 2;
    VEC3 vectorBetween = (points[i + 1] - points[i]);
    vectorBetween.x /= 2;
    vectorBetween.y /= 2;
    vectorBetween.z /= 2;
    cameraPositions[i] = points[i] + vectorBetween;
  }

  return true;
};

int TCompGuidedCamera::nearCameraPoint(VEC3 playerPosition) {
  int pos = -1;
  float latestInfluence = 999.9f;
  for (int i = 0; i < num_cameras; ++i) {
    float dist = realDist(playerPosition, cameraPositions[i]);
    if (dist < influences[i] && dist < latestInfluence) {
      latestInfluence = dist;
      pos = i;
    }
  }
  return pos;
};

CQuaternion TCompGuidedCamera::getNewRotationForCamera(VEC3 playerPosition, CQuaternion cameraActual, int pointOfInfluence) {
  cameraActual.Normalize();
  if (pointOfInfluence < 0 || pointOfInfluence >= num_cameras) {
    return cameraActual;
  }

  float dist = realDist(playerPosition, cameraPositions[pointOfInfluence]);
  float distanciaUnitaria = dist / influences[pointOfInfluence];
  distanciaUnitaria = (1 - distanciaUnitaria);
  CQuaternion cameraNova = CQuaternion::Slerp(cameraActual, rotations[pointOfInfluence], distanciaUnitaria);
  cameraNova.Normalize();
  return cameraNova;
};