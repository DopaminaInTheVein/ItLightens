#include "globals.fx"

//--------------------------------------------------------------------------------------
struct VS_TEXTURED_OUTPUT
{
  float4 Pos  : SV_POSITION;
  float2 UV   : TEXCOORD0;
  float3 wPos : TEXCOORD1;
  float4 ins1 : OUTPUTO;
  float4 color : COLOR1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_TEXTURED_OUTPUT VS(
  float4 Pos : POSITION0              // Stream 0
  , float2 UV : TEXCOORD0
  , float4 InstancePos : POSITION1    // Stream 1
  , float3  InstanceData : TEXCOORD1    // Stream 1
  , float3  InstanceRot : ROTATION1    // Stream 1
  , float4  InstanceColor : COLOR1
  )
{
  //instanceData.r = frames
  //instanceData.g = size
  //instanceData.b = quantity of animation frames

  float size = InstanceData.g;
  float nframe = InstanceData.r*InstancePos.w;
  float q_frames = InstanceData.b;	

  //float q_frames = 4;
  VS_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT)0;

  float cs = cos(InstanceRot);
  float sn = sin(InstanceRot);
  float2 rotPos;

  rotPos.x = Pos.x*cs - Pos.y*sn;
  rotPos.y = Pos.x*sn + Pos.y*cs;

  float3 wpos = InstancePos.xyz
    + (CameraUp.xyz * rotPos.y
      + CameraLeft.xyz * rotPos.x
      )*size;

  output.Pos = mul(float4(wpos, 1), ViewProjection);
  output.color = InstanceColor;

  // Animate the UV's. Assuming 4x4 frames
  float nmod16 = fmod(nframe * 32, 16.0);
  int   idx = int(nmod16);
  float coords_x = fmod(idx, q_frames);
  float coords_y = int(idx / q_frames);

  output.UV.x = (coords_x + UV.x) / q_frames;
  output.UV.y = (coords_y + UV.y) / q_frames;

  output.wPos = wpos;
  output.ins1 = float4(nframe, nframe, nframe, 1.0f);

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

    //return input.ins1;
//return float4(input.ins2.yyy, 1.0f);

float4 color = txDiffuse.Sample(samLinear, input.UV);
color.xyz *= input.color.xyz;
color.a *= input.color.a;
//color.a *= length(color.xyz);
//return float4(1,1,1,1);
return color;
}