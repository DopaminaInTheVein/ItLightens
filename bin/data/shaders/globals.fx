#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"
#include "constants/ctes_globals.h"
#include "constants/ctes_material.h"
<<<<<<< HEAD
=======
#include "constants/ctes_blur.h"
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427

Texture2D txDiffuse   : USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txNormal    : USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txDepths    : USE_SHADER_REG(TEXTURE_SLOT_DEPTHS);
Texture2D txLightMask : USE_SHADER_REG(TEXTURE_SLOT_LIGHT_MASK);
Texture2D txNoise     : USE_SHADER_REG(TEXTURE_SLOT_NOISE);
Texture2D txShadowMap : USE_SHADER_REG(TEXTURE_SLOT_SHADOWMAP);
Texture2D txMetallic  : USE_SHADER_REG(TEXTURE_SLOT_METALLIC);
Texture2D txRoughness : USE_SHADER_REG(TEXTURE_SLOT_ROUGHNESS);
Texture2D txSpecular  : USE_SHADER_REG(TEXTURE_SLOT_SPECULAR);

TextureCube txEnvironment : USE_SHADER_REG(TEXTURE_SLOT_ENVIRONMENT);
TextureCube txIrradiance  : USE_SHADER_REG(TEXTURE_SLOT_IRRADIANCE);

// Same order as
SamplerState samLinear : register(s0);
<<<<<<< HEAD
SamplerState samLightBlackBorder : register(s1);
SamplerComparisonState samPCFShadows : register(s2);
=======
SamplerState samClampLinear : register(s1);
SamplerState samLightBlackBorder : register(s2);
SamplerComparisonState samPCFShadows : register(s3);
>>>>>>> d6b4e6803fa82f01d5c091b986f30dbebbb8b427

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

