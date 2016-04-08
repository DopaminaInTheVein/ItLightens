#ifndef INC_DRAW_UTILS_H_
#define INC_DRAW_UTILS_H_

#include "render/shader_cte.h"

bool createDepthBuffer(
    int xres
  , int yres
  , DXGI_FORMAT depth_format
  , ID3D11Texture2D** out_depth_resource
  , ID3D11DepthStencilView** out_depth_stencil_view
  );

//void drawLine(const VEC3& src, const VEC3& dst, const VEC4& color);

#include "contants/ctes_camera.h"
extern CShaderCte< TCteCamera > shader_ctes_camera;
#include "contants/ctes_object.h"
extern CShaderCte< TCteObject > shader_ctes_object;
#include "contants/ctes_bones.h"
extern CShaderCte< TCteBones > shader_ctes_bones;

#endif