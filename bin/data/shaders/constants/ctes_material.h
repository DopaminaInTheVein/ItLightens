#ifndef INC_RENDER_CTS_MATERIAL_H_
#define INC_RENDER_CTS_MATERIAL_H_

#include "constants/ctes_platform.h"

ShaderBuffer(TCteMaterial, CTE_SHADER_MATERIAL_SLOT)
{
  float  scalar_roughness;
  float  scalar_metallic;
  float  scalar_irradiance_vs_mipmaps;
  float  scalar_dummy;
};

#endif
