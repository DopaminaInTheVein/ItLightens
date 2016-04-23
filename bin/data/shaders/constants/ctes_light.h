#ifndef INC_CTES_SHADER_LIGHT_
#define INC_CTES_SHADER_LIGHT_

#include "constants/ctes_platform.h"

ShaderBuffer(TCteLight, CTE_SHADER_LIGHT)
{
	float4 LightWorldPos;
	matrix LightViewProjection;
	float4 LightColor;
	float  LightInRadius;
	float  LightOutRadius;
	float  LightDummy1;
	float  LightDummy2;
};

#endif
