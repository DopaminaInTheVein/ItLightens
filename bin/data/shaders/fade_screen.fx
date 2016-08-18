#include "globals.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VSTextureScreen(
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
float4 PSFade(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	float4 o_color = float4(0,0,0,0);
	
	o_color.a += 1 * fade_black_screen;
	
	//o_color.a = 1 - o_color.a;
	
	return o_color;
}

//--------------------------------------------------------------------------------------
float4 PSDarken(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	float4 base_color = txDiffuse.Sample(samLinear, iTex0);
	float factor = 0.95f;
	float depth = txDepths.Sample(samLinear, iTex0).r;
	
	float4 rnd = txNoise.Sample(samLinear, iTex0+sin(world_time));
	float int_color = length(base_color.xyz)*factor;
	//float4 o_color = float4(int_color, int_color, int_color, int_color);
	//float4 o_color = float4(base_color.r*factor, base_color.g*factor, base_color.b*factor, base_color.a*factor);
	float4 o_color = base_color*factor;
	o_color.b -= o_color.b*0.9f;
	//o_color.a += 1 * fade_black_screen;
	
	//o_color.a = 1 - o_color.a;
	
	return o_color;
}

