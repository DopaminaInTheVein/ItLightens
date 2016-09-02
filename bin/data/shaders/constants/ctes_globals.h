#include "constants/ctes_platform.h"

ShaderBuffer(TCteGlobals, CTE_SHADER_GLOBALS_SLOT)
{
  float world_time;
  float xres;
  float yres;
  float dummy_global1;

  float current_life_player;
  float max_life_player;
  float polarity;
  float dummy_global3;

  float4 global_color;
};
