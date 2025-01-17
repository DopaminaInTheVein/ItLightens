#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"
#include "constants/ctes_globals.h"
#include "constants/ctes_hatching.h"
#include "constants/ctes_bones.h"
#include "constants/ctes_gui.h"
#include "constants/ctes_blur.h"
#include "constants/ctes_dream.h"
#include "constants/ctes_fog.h"

#define PI 3.1415926

Texture2D txDiffuse   	: USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txGloss	  	: USE_SHADER_REG(TEXTURE_SLOT_GLOSS);
Texture2D txNormal    	: USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txSpecular  	: USE_SHADER_REG(TEXTURE_SLOT_SPECULAR);
Texture2D txDepths    	: USE_SHADER_REG(TEXTURE_SLOT_DEPTHS);
Texture2D txLightMask 	: USE_SHADER_REG(TEXTURE_SLOT_LIGHT_MASK);
Texture2D txSelfIlum  	: USE_SHADER_REG(TEXTURE_SLOT_SELFILUM);
Texture2D txNoise     	: USE_SHADER_REG(TEXTURE_SLOT_NOISE);

Texture2D txSpeculars 	: USE_SHADER_REG(TEXTURE_SLOT_SPECULAR_GL);
Texture2D txGlossiness 	: USE_SHADER_REG(TEXTURE_SLOT_GLOSSINESS);
Texture2D txSpeculars_lights : USE_SHADER_REG(TEXTURE_SLOT_SPECULAR_LIGHTS);


//shadows
Texture2D txShadowMap 	: USE_SHADER_REG(TEXTURE_SLOT_SHADOWMAP);
Texture2D txShadowMapStatic : USE_SHADER_REG(TEXTURE_SLOT_SHADOWMAP_STATICS);



Texture2D txWarpLight 	: USE_SHADER_REG(TEXTURE_SLOT_RAMP);

//environment maps
TextureCube txHubEnv1	: USE_SHADER_REG(CUBEMAP_SLOT_HUB1);

TextureCube txMoleEnv1	: USE_SHADER_REG(CUBEMAP_SLOT_MOLE1);
TextureCube txMoleEnv2	: USE_SHADER_REG(CUBEMAP_SLOT_MOLE2);
TextureCube txMoleEnv3	: USE_SHADER_REG(CUBEMAP_SLOT_MOLE3);
TextureCube txMoleEnv4	: USE_SHADER_REG(CUBEMAP_SLOT_MOLE4);

TextureCube txSciEnv1	: USE_SHADER_REG(CUBEMAP_SLOT_SCI1);
TextureCube txSciEnv2	: USE_SHADER_REG(CUBEMAP_SLOT_SCI2);
TextureCube txSciEnv3	: USE_SHADER_REG(CUBEMAP_SLOT_SCI3);
TextureCube txSciEnv4	: USE_SHADER_REG(CUBEMAP_SLOT_SCI4);

//temporal data
Texture2D txData1		: USE_SHADER_REG(TEXTURE_SLOT_DATA1);
Texture2D txData2		: USE_SHADER_REG(TEXTURE_SLOT_DATA2);

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
  float ux = 1.0 - screen_coords.x / xres * 2;
  
  // Si screen_coords =   0 => uy = 1;
  // Si screen_coords = 400 => uy = 0;
  // Si screen_coords = 800 => uy = -1;
  float uy = 1.0 - screen_coords.y / yres * 2;
  
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

