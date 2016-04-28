#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"


Texture2D txDiffuse   : USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txNormal    : USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txWorldPos  : USE_SHADER_REG(TEXTURE_SLOT_WORLD_POS);
Texture2D txSelfIlum : USE_SHADER_REG(TEXTURE_SLOT_SELFILUM);

Texture2D txEnvironment : USE_SHADER_REG(TEXTURE_SLOT_ENVIRONMENT);

SamplerState samLinear : register(s0);
SamplerState samClampLinear : register(s2);


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
	float4 selfIlum = txSelfIlum.Sample(samLinear, iTex0);

	output = diff*0.2f + lights*0.8f;
	output.a = diff.a;

	output.rgb += selfIlum.rgb;
}

//--------------------------------------------------------------------------------------
struct VS_TEXTURED_OUTPUT
{
	float4 Pos    : POSITION;
	float2 UV     : TEXCOORD0;
	float4 wPos   : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VSBlur(
	in float4 iPos : POSITION
	, in float2 iTex0 : TEXCOORD0
	, out float4 oPos : SV_POSITION
	, out float2 oTex0 : TEXCOORD0
	, out float3 oWorldPos : TEXCOORD1
	)
{
	float4 worldPos = mul(iPos, World);
	oWorldPos = worldPos.xyz;
	oPos = mul(worldPos, ViewProjection);
	oTex0 = iTex0;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSBlur(float4 Pos : SV_POSITION
			, float2 iTex0 : TEXCOORD0
			, float3 iWorldPos : TEXCOORD1) : SV_Target
{
	float Pixels[13] =
	{
	   -6,
	   -5,
	   -4,
	   -3,
	   -2,
	   -1,
		0,
		1,
		2,
		3,
		4,
		5,
		6,
	};
	
	float BlurWeights[13] =
	{
		0.002216,
		0.008764,
		0.026995,
		0.064759,
		0.120985,
		0.176033,
		0.199471,
		0.176033,
		0.120985,
		0.064759,
		0.026995,
		0.008764,
		0.002216,
	};

    float pixelWidth = 1.0f/512.0f;

    float4 color = {0, 0, 0, 1};
	
	//float3 E = normalize(CameraWorldPos.xyz - iWorldPos);
	//float3 E = normalize(float3(0,0,0) - iWorldPos);
	
    float2 blur;
    blur.y = iTex0.y;
	
	
	//return float4(,1);
	//return float4(iWorldPos,1);
	//return float4(E,1);

    for (int i = 0; i < 13; i++) 
    {
        blur.x = iTex0.x + Pixels[i] * pixelWidth;
        color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
    }  


	blur.x = iTex0.x;
	for (int i = 0; i < 13; i++)
	{
		blur.y = iTex0.y + Pixels[i] * pixelWidth;
		color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
	}

    return color;
}
