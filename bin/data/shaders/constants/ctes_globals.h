#include "constants/ctes_platform.h"

ShaderBuffer(TCteGlobals, CTE_SHADER_GLOBALS_SLOT)
{
  float world_time;
  float xres;
  float yres;
  float fade_black_screen;

  float current_life_player;
  float max_life_player;
  float use_ramp_color;
  float use_ramp;

  float env_factor;
  float shadow_intensity;
  float polarity;
  float goblal_dummy;

  float4 global_color;
};
