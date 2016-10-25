#ifndef INC_CTES_SHADER_LIGHT_
#define INC_CTES_SHADER_LIGHT_

#include "constants/ctes_platform.h"

ShaderBuffer(TCteLight, CTE_SHADER_LIGHT)
{
  matrix LightViewProjection;
  matrix LightViewProjectionOffset;
  float4 LightWorldPos;
  float4 LightWorldFront;
  float4 LightColor;
  float  LightInRadius;
  float  LightOutRadius;
  float  LightAspectRatio;  // only directionals
  float  LightCosFov;

  float generate_shadows;
  float light_dummy1;
  float light_dummy2;
  float light_dummy3;
};

#endif
