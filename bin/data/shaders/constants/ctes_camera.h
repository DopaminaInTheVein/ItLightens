#include "constants/ctes_platform.h"

ShaderBuffer(TCteCamera, CTE_SHADER_CAMERA_SLOT)
{
  matrix ViewProjection;
  matrix InvViewProjection;
  float4 CameraWorldPos;
  float4 CameraFront;
  float4 CameraUp;
  float4 CameraLeft;

  float  CameraZFar;
  float  CameraZNear;
  float  CameraTanHalfFov;
  float  CameraAspectRatio;
};
