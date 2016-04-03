#include "contants/ctes_platform.h"

ShaderBuffer(TCteObject, CTE_SHADER_OBJECT_SLOT)
{
	matrix World;
	matrix Rotation;
	float4 lightvec;     
	float4 lightcol;      
	float4 ambientcol;  
	//float4 obj_color;
};
