#include "contants/ctes_platform.h"

ShaderBuffer(TCteCamera, CTE_SHADER_CAMERA_SLOT)
{
	float4 CamPosition;
	//matrix View;
	//matrix Projection;
	matrix ViewProjection;

	float4 lightvec;
	float4 lightcol;
	float4 ambientcol;
};
