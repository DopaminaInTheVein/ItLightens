#ifndef INC_CTES_SHADER_UI_
#define INC_CTES_SHADER_UI_

#include "constants/ctes_platform.h"

ShaderBuffer(TCteGui, CTE_SHADER_GUI_SLOT)
{
  float  state_ui;
  float  polarity_ui;
  float  dummy_ui1;  
  float  dummy_ui2;

  float	 pos_x;
  float	 pos_y;
  float  size_x;
  float  size_y;

  float4 color_ui;
};

#endif
