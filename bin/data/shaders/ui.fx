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
  return txDiffuse.Sample(samLinear, iTex0);
}
