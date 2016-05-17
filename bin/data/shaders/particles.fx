#include "globals.fx"

//--------------------------------------------------------------------------------------
struct VS_TEXTURED_OUTPUT
{
  float4 Pos  : SV_POSITION;
  float2 UV   : TEXCOORD0;
  float3 wPos : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_TEXTURED_OUTPUT VS(
  float4 Pos : POSITION0              // Stream 0
, float2 UV  : TEXCOORD0
, float3 InstancePos   : POSITION1    // Stream 1
, float  InstanceFrame : TEXCOORD1    // Stream 1
)
{
  VS_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT)0;
  
  float3 wpos = InstancePos
    + ( CameraUp.xyz * Pos.y
      + CameraLeft.xyz * Pos.x
      );
  output.Pos = mul(float4( wpos, 1 ), ViewProjection);
  
  // Animate the UV's. Assuming 4x4 frames
  float nmod16 = fmod(InstanceFrame*32, 16.0);
  int   idx = int(nmod16);
  float coords_x = fmod(idx, 4);
  float coords_y = int( idx / 4);

  output.UV.x = (coords_x + UV.x) / 4.0;
  output.UV.y = (coords_y + UV.y) / 4.0;

  output.wPos = wpos;

  return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_TEXTURED_OUTPUT input
  , in float4 iPosition : SV_Position
  ) : SV_Target
{
/*
  float my_depth = dot(input.wPos - CameraWorldPos, cameraWorldFront) / cameraZFar;
  
  int3 ss_load_coords = uint3(iPosition.xy, 0);
  
  float pixel_detph = txDepth.Load(ss_load_coords).x;

  float delta_z = abs(pixel_detph - my_depth);
  delta_z = saturate(delta_z * 1000);
  */

  float4 color = txDiffuse.Sample(samLinear, input.UV);
  color.a *= length(color.xyz);
  return color;
}

