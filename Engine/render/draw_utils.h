#ifndef INC_DRAW_UTILS_H_
#define INC_DRAW_UTILS_H_

#include "render/shader_cte.h"

class CCamera;

bool createDepthBuffer(
    int xres
  , int yres
  , DXGI_FORMAT depth_format
  , ID3D11Texture2D** out_depth_resource
  , ID3D11DepthStencilView** out_depth_stencil_view
  , const char* name
  , CTexture** out_ztexture = nullptr
  );

void drawLine(const VEC3& src, const VEC3& dst, const VEC4& color);
void drawFullScreen(const CTexture* texture, const CRenderTechnique* technique = nullptr);
void activateCamera(const CCamera* camera);
void activateWorldMatrix(const MAT44& mat);
void drawWiredAABB(const AABB& aabb, const MAT44& world, VEC4 color);
bool drawUtilsCreate();
void drawUtilsDestroy();
void activateDefaultStates();

#include "constants/ctes_camera.h"
extern CShaderCte< TCteCamera > shader_ctes_camera;
#include "constants/ctes_object.h"
extern CShaderCte< TCteObject > shader_ctes_object;
#include "constants/ctes_bones.h"
extern CShaderCte< TCteBones > shader_ctes_bones;
#include "constants/ctes_light.h"
extern CShaderCte< TCteLight > shader_ctes_lights;
#include "constants/ctes_globals.h"
extern CShaderCte< TCteGlobals > shader_ctes_globals;
#include "constants/ctes_blur.h"
extern CShaderCte< TCteBlur > shader_ctes_blur;
#include "constants/ctes_hatching.h"
extern CShaderCte< TCteHatching > shader_ctes_hatching;
#include "constants/ctes_gui.h"
extern CShaderCte< TCteGui > shader_ctes_gui;
#include "constants/ctes_dream.h"
extern CShaderCte< TCteDream > shader_ctes_dream;
#include "constants/ctes_fog.h"
extern CShaderCte< TCteFog > shader_ctes_fog;

extern const CTexture* all_black;

#endif