#ifndef INC_IK_HANDLER_H_
#define INC_IK_HANDLER_H_

#include "mcv_platform.h"

struct TIKHandle {
  float    AB;        // Given from Bone
  float    BC;        // Given from Bone
  VEC3 A, B, C;   // A & C are given, C is found
  float    h;         // Perp amount with respect to AC
  VEC3 normal;

  enum eState {
    NORMAL
  , TOO_FAR
  , TOO_SHORT
  , UNKNOWN
  };
  eState   state;

  bool solveB() {

    VEC3 dir = C - A;
    float AC = dir.Length();
	dir.Normalize();

    if (AC > AB + BC) {
      state = TOO_FAR;
      B = A + dir * AB;
      return false;
    }

    float num = AB*AB - BC*BC - AC*AC;
    float den = -2 * AC;

    if (den == 0) {
      state = UNKNOWN;
      return false;
    }

    float a2 = num / den;
    float a1 = AC - a2;

    // h^2 + a1^2 = AB^2
    float h2 = AB*AB - a1 * a1;
    if (h2 < 0.f) {
      state = TOO_SHORT;
      B = A - dir * AB;
      return false;
    }
    h = sqrtf(h2);

    VEC3 perp_dir = XMVector3Cross(normal, dir);
    perp_dir = XMVector3Normalize(perp_dir);
    B = A + dir * a1 + h * perp_dir;
    state = NORMAL;
    return true;
  }

};



#endif

