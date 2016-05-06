#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"
#include "constants/ctes_globals.h"


Texture2D txDiffuse   : USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txNormal    : USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txWorldPos  : USE_SHADER_REG(TEXTURE_SLOT_WORLD_POS);
Texture2D txSelfIlum : USE_SHADER_REG(TEXTURE_SLOT_SELFILUM);
Texture2D txDepth : register(t45);

Texture2D txEnvironment : USE_SHADER_REG(TEXTURE_SLOT_ENVIRONMENT);

SamplerState samLinear : register(s0);
SamplerState samClampLinear : register(s2);



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
float dc = txDepth.Sample(samLinear, iTex0).r;
//float pixelWidth = 1.0f;
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

    

    float4 color = {0, 0, 0, 0};
	
	//float3 E = normalize(CameraWorldPos.xyz - iWorldPos);
	//float3 E = normalize(float3(0,0,0) - iWorldPos);
	
    float2 blur;
    blur.y = iTex0.y;
	
	
	//return float4(,1);
	//return float4(iWorldPos,1);
	//return float4(E,1);
	
	color = txDiffuse.Sample(samLinear, iTex0.xy) ;
	return color;
	
	//pixelWidth = strenght_polarize/(xres)/dc;
	float2 pixelWidth = float2(1/xres, 1/yres);
	//pixelWidth = pixelWidth/dc;
	
	float sinw = 1.0f;
	float offset_c = sinw*sinw/2;
	offset_c = 1.0f;

    for (int i = 0; i < 13; i++) 
    {
        blur.x = iTex0.x + Pixels[i] * pixelWidth*offset_c;
        color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
    }  
	
	blur.x = iTex0.x;
	
	for (int i = 0; i < 13; i++)
	{
		blur.y = iTex0.y + Pixels[i] * pixelWidth*offset_c;
		color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
	}
	
	color.w = color.w/2.0f;
	
	//color *= float4(1.0f,0.3f,0.3f,1.0f);
	
	if (color.r < 0.3 && color.g < 0.3 && color.b < 0.3)
		color.w = 0;
		
	//color.w = color.w/1.5f;
		
    return color*offset_c;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSBlur_int(float4 Pos : SV_POSITION
			, float2 iTex0 : TEXCOORD0
			, float3 iWorldPos : TEXCOORD1) : SV_Target
{
float dc = txDepth.Sample(samLinear, iTex0).r;
float pixelWidth = 1.0f;
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

    

    float4 color = {0, 0, 0, 1};
	
	//float3 E = normalize(CameraWorldPos.xyz - iWorldPos);
	//float3 E = normalize(float3(0,0,0) - iWorldPos);
	
    float2 blur;
    blur.y = iTex0.y;
	
	
	//return float4(,1);
	//return float4(iWorldPos,1);
	//return float4(E,1);
	
	pixelWidth = strenght_polarize/(xres)/dc;
	
	float sinw = abs(sin(world_time/3));
	float offset_c = sinw*sinw/2;


    for (int i = 0; i < 13; i++) 
    {
        blur.x = iTex0.x + Pixels[i] * pixelWidth*offset_c;
        color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
    }  
	
	blur.x = iTex0.x;
	
	for (int i = 0; i < 13; i++)
	{
		blur.y = iTex0.y + Pixels[i] * pixelWidth*offset_c;
		color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
	}
	
	//color *= float4(1.0f,0.3f,0.3f,1.0f);
	
	if (color.r < 0.3 && color.g < 0.3 && color.b < 0.3)
		color.w = 0;
		
    return color*offset_c;
}



