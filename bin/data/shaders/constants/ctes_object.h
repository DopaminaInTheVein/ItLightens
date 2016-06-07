#include "constants/ctes_platform.h"

ShaderBuffer(TCteObject, CTE_SHADER_OBJECT_SLOT)
{
	matrix World;
	float4 obj_color;

	//polarity:
	// -1 -> -
	//  0 -> neutral/no polarity
	//  1 -> +
	float polarity;
	float dummy_object1;
	float dummy_object2;
	float dummy_object3;
};
