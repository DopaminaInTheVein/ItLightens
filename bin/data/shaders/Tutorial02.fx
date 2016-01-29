#include "contants/ctes_camera.h"
#include "contants/ctes_object.h"

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
  oPos = mul(worldPos, ViewProjection );
  oColor = iColor;
}

//--------------------------------------------------------------------------------------
void VS_UV( 
    in float4 iPos : POSITION
  , in float2 iTex0 : TEXCOORD0
  , out float4 oPos : SV_POSITION
  , out float4 oColor : COLOR 
  )
{
  float4 worldPos = mul(iPos, World);
  oPos = mul(worldPos, ViewProjection );
  oColor = float4(iTex0.xyx,1);   // en el blue poner la uv.x
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( float4 Pos : SV_POSITION
         , float4 iColor : COLOR 
         ) : SV_Target
{
    return iColor;
}
