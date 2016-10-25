#include "constants/ctes_platform.h"

ShaderBuffer(TCteBlur, CTE_SHADER_BLUR_SLOT)
{
  float4 blur_w;
  float4 blur_d;

  float2 blur_step;

  float ssao_intensity;
  float ssao_iterations;

  float ssao_sample_rad;
  float ssao_scale;
  float ssao_bias;
  float ssao_test_intensity;
};
