#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"


Texture2D txDiffuse   : USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txNormal    : USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txWorldPos  : USE_SHADER_REG(TEXTURE_SLOT_WORLD_POS);

Texture2D txEnvironment : USE_SHADER_REG(TEXTURE_SLOT_ENVIRONMENT);

SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VS(
	in float4 iPos : POSITION
	, in float4 iColor : COLOR
	, out float4 oPos : SV_POSITION
	, out float4 oColor : COLOR
	)
{
	float4 worldPos = mul(iPos, World);
	oPos = mul(worldPos, ViewProjection);
	oColor = iColor * obj_color;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION
	, float4 iColor : COLOR
	) : SV_Target
{
	return iColor;
}

//--------------------------------------------------------------------------------------
void VS_N_UV(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL
	, in float2 iTex0 : TEXCOORD0
	, out float4 oPos : SV_POSITION
	, out float2 oTex0 : TEXCOORD0
	)
{
	float4 worldPos = mul(iPos, World);
	oPos = mul(worldPos, ViewProjection);
	oTex0 = iTex0;
}

//--------------------------------------------------------------------------------------
// Vertex Shader for pos + uv vertexs types
//--------------------------------------------------------------------------------------
void VSTextured(
  in float4 iPos : POSITION
  , in float2 iTex0 : TEXCOORD0
  , out float4 oPos : SV_POSITION
  , out float2 oTex0 : TEXCOORD0
  )
{
  float4 worldPos = mul(iPos, World);
  oPos = mul(worldPos, ViewProjection);
  oTex0 = iTex0;
}

//--------------------------------------------------------------------------------------
float4 PSTextured(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	return txDiffuse.Sample(samLinear, iTex0);
}

//--------------------------------------------------------------------------------------
void PSAddAmbient(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	, out float4 output : SV_Target
	) 
{

	float4 diff = txDiffuse.Sample(samLinear, iTex0);
	float4 lights = txEnvironment.Sample(samLinear, iTex0);
	float4 normals = txNormal.Sample(samLinear, iTex0); 

	output = diff*0.2f + lights*0.8f;
	output.a = diff.a;
}

