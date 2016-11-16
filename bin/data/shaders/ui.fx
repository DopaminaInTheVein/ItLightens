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
	//result += float4(0,0.5,0,1)*result.a*result.r;
  }
  else if(polarity == 2.0f){
	result *= txDiffuse.Sample(samLinear, iTex0);
	//result *= float4(1,1,1,0.5);
	result *= float4(0.3,0.3,0.3,1);
	//result += float4(-0.7,0.1,0,-0.5)*result.a*length(result.rgb);
  }
  else{
	result *= txDiffuse.Sample(samLinear, iTex0);
	result *= float4(0.5,0.5,0.5,1);
	//result += float4(-0.5,0.1,0,1)*result.a*result.r;
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
	result.rgba *= txDiffuse.Sample(samLinear, iTex0*0.9 + 0.04);
	result.rgb *= float4(0.5,0.5,1,1)*result.a*result.b;
  }
  else if(polarity == 1.0f){
	result *= txDiffuse.Sample(samLinear, iTex0);
	
	result.rgb *= float4(0.5,0.5,1,1)*result.a*result.b;
	result *= float4(0.4,0.4,0.4,1);
  }
  else{
	result *= txDiffuse.Sample(samLinear, iTex0);
	result.rgb *= float4(0.5,0.5,1,1)*result.a*result.b;
	//result.rgb *= float4(0.3,0.4,1,1)*result.a*result.b;
	result *= float4(0.6,0.6,0.6,1);
  }
  return result;
  
  //return txDiffuse.Sample(samLinear, iTex0);
}

//defined on gui_button.cpp
#define RSTATE_DISABLED	-1.f
#define RSTATE_ENABLED	0.f
#define RSTATE_OVER		1.f
#define RSTATE_CLICKED	2.f
#define RSTATE_RELEASED	3.f

static float base_color = 0.5f;

float4 PSButton(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
) : SV_Target
{
  //float4 result = txDiffuse.Sample(samLinear, iTex0);
  float2 min_coord = float2(pos_x, pos_y);
  float2 max_coord = float2(min_coord.x+size_x, min_coord.y+size_y);
  float2 tex = min_coord + iTex0 * (max_coord - min_coord);
  float4 result = txDiffuse.Sample(samLinear, tex);
  result.rgb = result.rgb*base_color;
  float alpha = result.a;
  if(state_ui == RSTATE_DISABLED){
	result *= float4(0.5,0.5,0.5,1);
  }else if(state_ui == RSTATE_ENABLED){
	float influence = state_ui - RSTATE_ENABLED + 1;
	float inv_influence = 1 - influence;
	result += base_color*(result);
  }else if(state_ui <= RSTATE_OVER){
	float influence = state_ui - RSTATE_OVER + 1;
	float inv_influence = 1 - influence;
	result += base_color*(result*inv_influence + float4(1,1,0,1)*state_ui);
  }else if(state_ui <= RSTATE_CLICKED){
	float influence = state_ui - RSTATE_CLICKED + 1;
	float inv_influence = 1 - influence;
	result += base_color*(result*inv_influence + float4(1,0.5,0,1)*state_ui);
  }else{
	float influence = state_ui - RSTATE_RELEASED + 1;
	float inv_influence = 1 - influence;
	result += base_color*(result*inv_influence + float4(1,0,0,1)*state_ui);
  }
  
  result.a = alpha;
  return result;
  
  //return txDiffuse.Sample(samLinear, iTex0);
}

bool isInside(float2 maxf, float2 minf, float2 pointf){
	if(maxf.x < pointf.x || maxf.y < pointf.y)
		return false;
		
	if(minf.x > pointf.x || minf.y > pointf.y)
		return false;
		
	return true;

}

float4 PSFont(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
) : SV_Target
{
  //float4 result = txDiffuse.Sample(samLinear, iTex0);
  float2 min_coord = float2(pos_x, pos_y);
  float2 max_coord = float2(min_coord.x+size_x, min_coord.y+size_y);
  float2 tex = min_coord + iTex0 * (max_coord - min_coord);
  float4 result = txDiffuse.Sample(samLinear, tex);
  //if(!isInside(max_coord, min_coord, iTex0))
  //result.a = 0;
  
  return result*color_ui;
  
  //return txDiffuse.Sample(samLinear, iTex0);
}


float4 PSBar(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
) : SV_Target
{
  float4 result = txDiffuse.Sample(samLinear, iTex0);

	if(result.a > 0.001f){
		//float life_normalized = state_ui;
		//result.a = 1 - result.a;
		result.a = step(result.a, state_ui);
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
