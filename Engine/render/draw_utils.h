#ifndef INC_DRAW_UTILS_H_
#define INC_DRAW_UTILS_H_

#include "render/shader_cte.h"
#include "contants/ctes_object.h"
extern CShaderCte< TCteObject > shader_ctes_object;

void drawLine(const VEC3& src, const VEC3& dst, const VEC4& color);

#endif