#include "constants/ctes_platform.h"

ShaderBuffer(TCteCamera, CTE_SHADER_CAMERA_SLOT)
{
	matrix ViewProjection;
  float4 CameraWorldPos;
  float4 CameraFront;

  float  CameraZFar;
  float  CameraZNear;
  float  CameraFov;
  float  CameraAspectRatio;
};
