#ifndef INC_COMPONENT_CULLING_H_
#define INC_COMPONENT_CULLING_H_

#include "geometry/geometry.h"
#include "components/comp_base.h"
#include <bitset>

struct TCompCulling : public TCompBase {
  std::bitset<4096> bits;

  // A single plane
  struct CPlane {
    VEC3  n;
    float d;
    void from(VEC4 k) {
      n = VEC3(k.x, k.y, k.z);
      d = k.w;
    }
  };

  // the 6 sides of the frustum
  enum EFrustumPlanes {
    FP_LEFT = 0
  , FP_RIGHT
  , FP_BOTTOM
  , FP_TOP
  , FP_NEAR
  , FP_FAR
  , FP_NPLANES
  };

  // An array of N planes
  class VPlanes : std::vector< CPlane > {
  public:
    void fromViewProjection(MAT44 view_proj) {
      resize(FP_NPLANES);
      MAT44 m = view_proj.Transpose();
      VEC4 mx = VEC4(m.m[0]);
      VEC4 my = VEC4(m.m[1]);
      VEC4 mz = VEC4(m.m[2]);
      VEC4 mw = VEC4(m.m[3]);
      VPlanes &vp = *this;
      vp[FP_LEFT].from(mw + mx);
      vp[FP_RIGHT].from(mw - mx);
      vp[FP_BOTTOM].from(mw + my);
      vp[FP_TOP].from(mw - my);
      vp[FP_NEAR].from(mw + mz);      // + mz if frustum is 0..1
      vp[FP_FAR].from(mw - mz);
    }
  };

  VPlanes planes;

  void renderInMenu();
  void update( MAT44 view_proj );
};


#endif
