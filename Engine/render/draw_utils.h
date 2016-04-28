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
	);

//void drawLine(const VEC3& src, const VEC3& dst, const VEC4& color);
void drawFullScreen(const CTexture* texture, const CRenderTechnique* technique = nullptr);
void activateCamera(const CCamera* camera);
void activateWorldMatrix(const MAT44& mat);
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

extern const CTexture* all_black;

#endif