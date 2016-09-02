#include "globals.fx"

//--------------------------------------------------------------------------------------
void VS(
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
float4 PS(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
) : SV_Target
{
  float4 result = txDiffuse.Sample(samLinear, iTex0);
  //result.a = 0;
  return result;
  
  //return txDiffuse.Sample(samLinear, iTex0);
}

float4 PSQPolarity(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
) : SV_Target
{
  float4 result = txDiffuse.Sample(samLinear, iTex0);
  //result.a = 0;
  
  if(polarity == 1.0f){
	result *= txDiffuse.Sample(samLinear, iTex0*0.9 + 0.04);
	result *= float4(1,1,0.5,1);
  }
  else if(polarity == 2.0f){
	result *= txDiffuse.Sample(samLinear, iTex0);
	result *= float4(1,1,1,0.5);
  }
  else{
	result *= txDiffuse.Sample(samLinear, iTex0);
  }
  
  return result;
  
  //return txDiffuse.Sample(samLinear, iTex0);
}


float4 PSEPolarity(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
) : SV_Target
{
  float4 result = float4(1,1,1,1); 
  //result.a = 0;
  
  if(polarity == 2.0f){
	result *= txDiffuse.Sample(samLinear, iTex0*0.9 + 0.04);
	result *= float4(1,1,0.5,1);
  }
  else if(polarity == 1.0f){
	result *= txDiffuse.Sample(samLinear, iTex0);
	result *= float4(1,1,1,0.5);
  }
  else{
	result *= txDiffuse.Sample(samLinear, iTex0);
  }
  return result;
  
  //return txDiffuse.Sample(samLinear, iTex0);
}


float4 PSHealthBar(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
) : SV_Target
{
  float4 result = txDiffuse.Sample(samLinear, iTex0);
  //result.a = 0;
  result.rgb *= float3(1,0.7,0.7);
  if(result.a > 0.01f){
		float life_normalized = current_life_player/max_life_player;
		result.a = 1 - result.a;
		result.a = step(result.a, life_normalized);
  }
  return result;
  
  //return txDiffuse.Sample(samLinear, iTex0);
}
