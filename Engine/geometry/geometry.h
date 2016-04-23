#ifndef INC_GEOMETRY_H_
#define INC_GEOMETRY_H_

#include "SimpleMath.h"
#include "SimpleMath.inl"

typedef DirectX::SimpleMath::Matrix  MAT44;
typedef DirectX::SimpleMath::Vector2 VEC2;
typedef DirectX::SimpleMath::Vector3 VEC3;
typedef DirectX::SimpleMath::Vector4 VEC4;
typedef DirectX::SimpleMath::Quaternion CQuaternion;

#define deg2rad( _deg )     ( _deg * (float)M_PI / 180.f )
#define rad2deg( _rad )     ( _rad  * 180.f / (float)M_PI )

#include "angular.h"
#include "transform.h"

#endif
