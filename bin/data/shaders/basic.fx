#include "contants/ctes_camera.h"
#include "contants/ctes_object.h"

Texture2D txDiffuse : register(t0);
Texture2D txBump: register(t1);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// STRUCTS
//--------------------------------------------------------------------------------------

struct VS_N_UV_normals_struct {
	float4 Pos		: SV_POSITION;
	float2 Tex     : TEXCOORD0;
	float3 Normal	: NORMAL;
};

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
	oCol += -float4(0, 0, 0, 0.7);
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



//textured VS & PS with normal map

//--------------------------------------------------------------------------------------
VS_N_UV_normals_struct VS_N_UV_normals(
	in float4 iPos : POSITION
	, in float3 iNor : NORMAL
	, in float2 iTex0 : TEXCOORD0
	, out float3 olightVector: LIGHT
	, out float3 oViewVec : VIEW
	)
{
	VS_N_UV_normals_struct output = (VS_N_UV_normals_struct)0;
	oViewVec = normalize(CamPosition - iPos);
	float4 worldPos = mul(iPos, World);
	output.Pos = mul(worldPos, ViewProjection);
	output.Normal = mul(iNor, World);
	output.Tex = iTex0;
	olightVector = lightvec;
	return output;
}


//--------------------------------------------------------------------------------------
float4 PSTextured_normals(VS_N_UV_normals_struct input
	, in float3 iLightVector : LIGHT
	, in float3 iViewVec : VIEW
	) : SV_Target
{

	float3 L = iLightVector;
	//float3 normal = txBump.Sample(samLinear, input.Tex).xyz * 2 - 0.5f;	//range -1 to 1
	float3 normal = txBump.Sample(samLinear, input.Tex).xyz;	//range -1 to 1
	normal = 1 - normal;	//¿?
	float3 ViewDir = normalize(iViewVec);
	float4 color = txDiffuse.Sample(samLinear, input.Tex);
	float3 LightDir = normalize(L); // L
	float4 diff = saturate(dot(normal, LightDir)); // diffuse comp.
	float4 eff = saturate(color * diff);

	float3 Reflect = normalize(4 * diff * normal - LightDir); // R
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 8)*0.5f+ eff*0.5f; // R.V^n 

	eff.a = 1;
	float shadow = saturate(diff);
	//return eff;
	return   float4(0.1 * color.xyz, 1) + shadow*eff+specular*0.5f;

}