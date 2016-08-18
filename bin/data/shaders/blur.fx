#include "globals.fx"

//--------------------------------------------------------------------------------------
void VS(
    in float4 iPos : POSITION
  , in float2 iTex0 : TEXCOORD0
  , out float4 oPos : SV_POSITION
  , out float2 oTex0 : TEXCOORD0
  , out float4 oTex1 : TEXCOORD1
  , out float4 oTex2 : TEXCOORD2
  , out float4 oTex3 : TEXCOORD3
)
{
  // Passthrough of coords and UV's
  oPos = float4(iPos.xyz, 1);
  oTex0 = iTex0;
  // Save in the xy the positive offset 
  // Save in the zw the negative offset 
  oTex1.xy = iTex0 + blur_step * blur_d.x;
  oTex1.zw = iTex0 - blur_step * blur_d.x;
  oTex2.xy = iTex0 + blur_step * blur_d.y;
  oTex2.zw = iTex0 - blur_step * blur_d.y;
  oTex3.xy = iTex0 + blur_step * blur_d.z;
  oTex3.zw = iTex0 - blur_step * blur_d.z;
}

//--------------------------------------------------------------------------------------
float4 PS(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
  , in float4 iTex1 : TEXCOORD1
  , in float4 iTex2 : TEXCOORD2
  , in float4 iTex3 : TEXCOORD3
) : SV_Target
{

 float factor = 1.5f;

  // 7 tap blur controlled by the vs
  float4 cp3 = txDiffuse.Sample(samClampLinear, iTex3.zw);
  float4 cp2 = txDiffuse.Sample(samClampLinear, iTex2.zw);
  float4 cp1 = txDiffuse.Sample(samClampLinear, iTex1.zw);
  float4 c0  = txDiffuse.Sample(samClampLinear, iTex0.xy)*factor;
  float4 cn1 = txDiffuse.Sample(samClampLinear, iTex1.xy);
  float4 cn2 = txDiffuse.Sample(samClampLinear, iTex2.xy);
  float4 cn3 = txDiffuse.Sample(samClampLinear, iTex3.xy);

  float4 cfinal =
    c0 * blur_w.x
    + (cp1 + cn1) * blur_w.y
    + (cp2 + cn2) * blur_w.z
    + (cp3 + cn3) * blur_w.w
    ;
	
  return cfinal;
}

//--------------------------------------------------------------------------------------
float4 PSBlurWithDepthVision(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
  , in float4 iTex1 : TEXCOORD1
  , in float4 iTex2 : TEXCOORD2
  , in float4 iTex3 : TEXCOORD3
) : SV_Target
{

  float depth = txDepths.Sample(samLinear, iTex0.xy);
  
  float factor = (1 - depth);
  //factor = 1;
	//factor = 0;

  // 7 tap blur controlled by the vs
  float4 cp3 = txDiffuse.Sample(samClampLinear, iTex3.zw)*factor;
  float4 cp2 = txDiffuse.Sample(samClampLinear, iTex2.zw)*factor;
  float4 cp1 = txDiffuse.Sample(samClampLinear, iTex1.zw)*factor;
  float4 c0  = txDiffuse.Sample(samClampLinear, iTex0.xy);
  float4 cn1 = txDiffuse.Sample(samClampLinear, iTex1.xy)*factor;
  float4 cn2 = txDiffuse.Sample(samClampLinear, iTex2.xy)*factor;
  float4 cn3 = txDiffuse.Sample(samClampLinear, iTex3.xy)*factor;

  float4 cfinal =
    c0 * blur_w.x
    + (cp1 + cn1) * blur_w.y
    + (cp2 + cn2) * blur_w.z
    + (cp3 + cn3) * blur_w.w
    ;

	cfinal *= float4(0.7,0.7f,1.0f,1);
	//return float4(depth, depth, depth, 1.0f);
  return cfinal;
}

//--------------------------------------------------------------------------------------
float4 PSBlurWithDepth(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
  , in float4 iTex1 : TEXCOORD1
  , in float4 iTex2 : TEXCOORD2
  , in float4 iTex3 : TEXCOORD3
) : SV_Target
{

  float depth = txDepths.Sample(samLinear, iTex0.xy);
  
  float factor = (1 - depth);
  //factor = 1;
	//factor = 0;

  // 7 tap blur controlled by the vs
  float4 cp3 = txDiffuse.Sample(samClampLinear, iTex3.zw)*factor;
  float4 cp2 = txDiffuse.Sample(samClampLinear, iTex2.zw)*factor;
  float4 cp1 = txDiffuse.Sample(samClampLinear, iTex1.zw)*factor;
  float4 c0  = txDiffuse.Sample(samClampLinear, iTex0.xy);
  float4 cn1 = txDiffuse.Sample(samClampLinear, iTex1.xy)*factor;
  float4 cn2 = txDiffuse.Sample(samClampLinear, iTex2.xy)*factor;
  float4 cn3 = txDiffuse.Sample(samClampLinear, iTex3.xy)*factor;

  float4 cfinal =
    c0 * blur_w.x
    + (cp1 + cn1) * blur_w.y
    + (cp2 + cn2) * blur_w.z
    + (cp3 + cn3) * blur_w.w
    ;

	cfinal *= float4(0.0,0.7f,1.0f,1);
	//return float4(depth, depth, depth, 1.0f);
  return cfinal;
}

