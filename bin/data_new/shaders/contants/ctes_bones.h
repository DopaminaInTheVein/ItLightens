#include "contants/ctes_platform.h"

#define MAX_BONES_SUPPORTED     100

ShaderBuffer(TCteBones, CTE_SHADER_BONES_SLOT)
{
  matrix Bones[MAX_BONES_SUPPORTED];
};

