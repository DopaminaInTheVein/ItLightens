#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"
#include "constants/ctes_globals.h"
#include "constants/ctes_hatching.h"

Texture2D txDiffuse   : USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txNormal    : USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txDepths    : USE_SHADER_REG(TEXTURE_SLOT_DEPTHS);
Texture2D txLightMask : USE_SHADER_REG(TEXTURE_SLOT_LIGHT_MASK);
Texture2D txSelfIlum  : USE_SHADER_REG(TEXTURE_SLOT_SELFILUM);
Texture2D txNoise     : USE_SHADER_REG(TEXTURE_SLOT_NOISE);
Texture2D txShadowMap : USE_SHADER_REG(TEXTURE_SLOT_SHADOWMAP);

Texture2D txEnvironment : USE_SHADER_REG(TEXTURE_SLOT_ENVIRONMENT);

Texture2D txWarpLight : register(t70);

// Same order as
SamplerState samLinear : register(s0);
SamplerState samLightBlackBorder : register(s1);
SamplerState samClampLinear : register(s2);
SamplerComparisonState samPCFShadows : register(s3);

// ------------------------------------------------------
// screen_coords va entre 0..1024
float3 getWorldCoords(float2 screen_coords, float zlinear_normalized) {

  // ux = -1 .. 1
  // Si screen_coords == 0 => ux = 1
  // Si screen_coords == 512 => ux = 0
  // Si screen_coords == 1024 => ux = -1
  float ux = 1.0 - screen_coords.x / 1024. * 2;
  
  // Si screen_coords =   0 => uy = 1;
  // Si screen_coords = 400 => uy = 0;
  // Si screen_coords = 800 => uy = -1;
  float uy = 1.0 - screen_coords.y / 800. * 2;
  
  float3 view_dir = float3( ux * CameraTanHalfFov * CameraAspectRatio
                          , uy * CameraTanHalfFov
                          , 1.) * ( zlinear_normalized * CameraZFar );

  float3 wPos =
      CameraFront.xyz * view_dir.z
    + CameraLeft.xyz  * view_dir.x
    + CameraUp.xyz    * view_dir.y
    + CameraWorldPos.xyz;
  return wPos;
  
}

