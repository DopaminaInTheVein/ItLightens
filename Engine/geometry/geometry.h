#ifndef INC_GEOMETRY_H_
#define INC_GEOMETRY_H_

#include "SimpleMath.h"
#include "SimpleMath.inl"

typedef DirectX::SimpleMath::Matrix  MAT44;
typedef DirectX::SimpleMath::Vector2 VEC2;
typedef DirectX::SimpleMath::Vector3 VEC3;
typedef DirectX::SimpleMath::Vector4 VEC4;
typedef DirectX::SimpleMath::Quaternion CQuaternion;
typedef DirectX::BoundingBox AABB;

#define deg2rad( _deg )     ( _deg * (float)M_PI / 180.f )
#define rad2deg( _rad )     ( _rad  * 180.f / (float)M_PI )

#define VEC3_UP VEC3(0.f, 1.f, 0.f)
#define VEC3_DOWN VEC3(0.f, -1.f, 0.f)
#define VEC3_LEFT VEC3(-1.f, 0.f, 0.f)
#define VEC3_RIGHT VEC3(1.f, 0.f, 0.f)
#define VEC3_FRONT VEC3(0.f, 0.f, 1.f)
#define VEC3_BACK VEC3(0.f, 0.f, -1.f)
#define VEC3_FROM_VALUE(v) (VEC3(v, v, v))

#include "angular.h"
#include "transform.h"

#endif
