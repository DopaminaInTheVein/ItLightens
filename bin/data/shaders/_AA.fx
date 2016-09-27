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



void VS_AA(
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



const static float2 offs[7] = {
   float2( 0.0,  0.0), //Center       0
   float2(-1.0, -1.0), //Top Left     1
   float2( 1.0,  1.0), //Bottom Right 5
   float2( 1.0, -1.0), //Top Right    3
   float2(-1.0,  1.0), //Bottom Left  7
   float2(-1.0,  0.0),  //Left         8
   float2( 0.0, -1.0) //Top          2
}; 

float4 PS_AA(float4 Pos : SV_POSITION
	, float2 uv : TEXCOORD0) : SV_TARGET

{
//return float4(0,1,1,1);
	float dc = txDepth.Sample(samLinear, uv).r;
	if(dc > 30.0f) 
		return txDiffuse.Sample(samClampLinear, uv);

	float2 pixel_size = float2((1.0f/xres),(1.0f/yres));

	float2 e_barrier = float2(0.5f,0.1f);  // x=norm, y=depth
	//float pixel_size = 1/2048.0f;	//to adjust line witdh
	//float pixel_size = 1.0f;
	float2 e_weights= float2(1,1);  // x=norm, y=depth

	float2 e_kernel = float2(1,1);   // x=norm, y=depth

	 
	
	 //pixel_size = pixel_size/dc; //adjust line witdh based on depth
	 float2 tc5r = -offs[5];

	 float2 tc6r = -offs[6];


	//dc = dc/32.0f; 
	//return float4(dc,dc,dc,1);
	//return float4(1,1,1,1);
	 float4 dd;

	 //float2 test = float2(10,10);
	 //return float4(uv.yyyyy);
	 dd.x = txDepth.Sample(samLinear, uv + offs[1]*pixel_size).r + txDepth.Sample(samLinear, uv + offs[2]*pixel_size).r;
	 dd.y = txDepth.Sample(samLinear, uv + offs[3]*pixel_size).r+txDepth.Sample(samLinear, uv + offs[4]*pixel_size).r;
	 dd.z = txDepth.Sample(samLinear, uv + offs[5]*pixel_size).r + txDepth.Sample(samLinear, uv + tc5r*pixel_size).r;
	 dd.w = txDepth.Sample(samLinear,uv +  offs[6]*pixel_size).r+ txDepth.Sample(samLinear,uv +  tc6r*pixel_size).r;

	//return dd;
	float4 a = (2*dc - dd);
	//return a;
	 dd = abs(2*dc - dd)- e_barrier.y;
	 //return dd;

	 dd = step( dd, 0);
	//return dd;
	 float de = saturate(dot(dd, e_weights));
	//return float4(de,de,de,1);
	 // Weight

	 float w = (1 - de) * e_kernel.x; 
//return float4(1,0,1,1);
//w = step(0.75f, w);

	 //return float4(w,w,w,1); ///stop here for an edge detector shader

	// The AA part (in edges we mix close pixels, non edge pixel w = 0 => use the normal uv, on edge we use a mean of four deslocated uv texture reads )
	// Smoothed color

	 // (a-c)*w + c = a*w + c(1-w)

 float2 offset = (uv ) * (1-w);
 float4 s0 = txDiffuse.Sample(samClampLinear, offset + (uv + offs[1] * pixel_size) * w);
 float4 s1 = txDiffuse.Sample(samClampLinear, offset + (uv + offs[2]* pixel_size) * w);
 float4 s2 = txDiffuse.Sample(samClampLinear, offset + (uv + offs[3]* pixel_size) * w);
 float4 s3 = txDiffuse.Sample(samClampLinear, offset + (uv + offs[4]* pixel_size) * w);

 return (s0 + s1 + s2 + s3)/4.h;
}
