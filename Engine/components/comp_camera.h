#ifndef INC_COMPONENT_CAMERA_H_
#define INC_COMPONENT_CAMERA_H_

#include "comp_base.h"
#include "camera/camera.h"

class CEntity;
struct TMsgGetCullingViewProj;

#define MAX_GUIDE_POINTS 64

// ------------------------------------
struct TCompCamera : public CCamera, public TCompBase {
  bool detect_colsions;
  float smoothDefault;
  float smoothCurrent;

  //Guided camera
  CHandle guidedCamera;
  //bool guidedCamera;
  //int lastguidedCamPoint;
  //VEC3 guided_positions[MAX_GUIDE_POINTS];
  //CQuaternion guided_rotations[MAX_GUIDE_POINTS];
  
  void render() const;
  void update(float dt);
  bool checkColision(const VEC3& pos);
  bool load(MKeyValue& atts);
  void renderInMenu();
  void updateFromEntityTransform(CEntity* e_owner);
  void onGetViewProj(const TMsgGetCullingViewProj& msg);

  void onGuidedCamera(const TMsgGuidedCamera&);
};

#endif
