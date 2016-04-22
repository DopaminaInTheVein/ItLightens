#include "contants/ctes_camera.h"
#include "contants/ctes_object.h"

Texture2D txDiffuse : register(t0);
Texture2D txBump: register(t1);
Texture2D txSpec: register(t2);
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
	, out float3 oLightVec : LIGHTVEC
	, out float3 oViewVec : VIEWVEC
	)
{
	VS_N_UV_normals_struct output;

	float4 worldPos = mul(iPos, World);
	output.Pos = mul(worldPos, ViewProjection);

	oViewVec = normalize(CamPosition - output.Pos);
	oViewVec = normalize(CamPosition - output.Pos);
	oLightVec = normalize(lightvec);

	output.Normal = mul(iNor, World);
	output.Tex = iTex0;

	return output;
}


//--------------------------------------------------------------------------------------
float4 PSTextured_normals(VS_N_UV_normals_struct input
	, in float3 iLightVec : LIGHTVEC
	, in float3 iViewVec : VIEWVEC
	) : SV_Target
{

	float3 ViewDir = iViewVec;
	float3 normal = txBump.Sample(samLinear, input.Tex);
	//return float4(normal,1);
	//normal = 1 - normal;	//invert values
	normal = normal*2 - 1.0f;	//range -1 to 1
	normal = normalize(normal);
	//return float4(normal, 1);

	float3 LightDir = iLightVec;


	float diffIntensity = dot(normal, LightDir);

	float4 color = txDiffuse.Sample(samLinear, input.Tex);
	//return float4(color.a, color.a, color.a, color.a);

	float4 color_final = saturate(color*diffIntensity);

	// ambient as base color * 0.1
	color_final = color_final + color*0.1f;

	

	//specular calulation
	if (diffIntensity > 0.0f) {

		float3 r = normalize(2 * dot(LightDir, normal) * normal - LightDir);
		float3 v = normalize(mul(normalize(ViewDir), World));

		

		//float4 SpecularColor = float4(1, 1, 1, 1);
		float4 SpecularColor = txSpec.Sample(samLinear, input.Tex);
		//return txSpec.Sample(samLinear, input.Tex);

		float SpecularIntensity = 2.0f;
		float Shininess = 20;

		float dotProduct = dot(r, v);
		float4 specular = SpecularIntensity * SpecularColor * max(pow(diffIntensity, Shininess), 0) * length(color);
		specular.a = color.a;

		color_final = saturate(color_final + specular);
		//return specular;
	}

	//transparency as alpha channel
	color_final.a = color.a;

	return  color_final;

}