#include "contants/ctes_camera.h"
#include "contants/ctes_object.h"

Texture2D txDiffuse : register(t0);
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
	oColor = iColor;// *obj_color;
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
	, out float4 oCol : COLOR
	, out float2 oTex0 : TEXCOORD0
	)
{
	float4 worldPos = mul(iPos, World);
	oPos = mul(worldPos, ViewProjection);

	oCol = ambientcol;		//ambient as base color

	float4 norm = normalize(mul(Rotation, iNormal));			//direction of light normalized on actual rotation
	float diffusebrightness = saturate(dot(norm, lightvec));
	//float diffusebrightness = saturate(dot(iNormal, lightvec));
	oCol += lightcol * diffusebrightness;

	oTex0 = iTex0;
}

//--------------------------------------------------------------------------------------
float4 PSTextured(float4 Pos : SV_POSITION
	, float4 iCol : COLOR
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	//apply color effect
  return txDiffuse.Sample(samLinear, iTex0) * iCol;
}

//--------------------------------------------------------------------------------------
void v_crystal(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL
	, in float2 iTex0 : TEXCOORD0
	, out float4 oPos : SV_POSITION
	, out float4 oCol : COLOR
	, out float2 oTex0 : TEXCOORD0
	)
{
	float4 worldPos = mul(iPos, World);
	oPos = mul(worldPos, ViewProjection);

	oCol = ambientcol;		//ambient as base color

	//float4 norm = normalize(mul(Rotation, iNormal));			//direction of light normalized on actual rotation
	//float diffusebrightness = saturate(dot(norm, lightvec));
	float diffusebrightness = saturate(dot(float3(0, 0, 0), lightvec));
	oCol += lightcol * diffusebrightness;
	oCol += -float4(0, 0, 0, 0.5);
	oTex0 = iTex0;
}

//--------------------------------------------------------------------------------------
void VS_NOSHADOW_TEX(
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
float4 PSTexturedNoShadow(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	//apply color effect
	return txDiffuse.Sample(samLinear, iTex0);
}