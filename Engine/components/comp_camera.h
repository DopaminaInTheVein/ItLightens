#ifndef INC_COMPONENT_CAMERA_H_
#define INC_COMPONENT_CAMERA_H_

#include "comp_base.h"
#include "camera/camera.h"

class CEntity;
struct TMsgGetCullingViewProj;

// ------------------------------------
struct TCompCamera : public CCamera, public TCompBase {
  bool detect_colsions;
  int lastguidedCamPoint = 0;
  float factor = 0.0f;

  void render() const;
  void update(float dt);
  bool checkColision(const VEC3& pos);
  bool load(MKeyValue& atts);
  void renderInMenu();
  void updateFromEntityTransform(CEntity* e_owner);
  void onGetViewProj(const TMsgGetCullingViewProj& msg);
};

#endif
