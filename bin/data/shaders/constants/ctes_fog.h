#include "constants/ctes_platform.h"

ShaderBuffer(TCteFog, CTE_SHADER_FOG_SLOT)
{
  float4 color_fog;

  float fog_density;
  float fog_upper_limit;
  float fog_max_intesity;
  float fog_distance;

  float fog_floor;
  float fog_dummy1;
  float fog_dummy2;
  float fog_dummy3;
};
