#include "globals.fx"

Texture2D txShadows : register(t78);

//--------------------------------------------------------------------------------------
// Vertex Shader for pos + uv vertexs types
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
float4 PS(float4 iPos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{

	int NUM_SAMPLES = 10; 
	float illuminationDecay = 1.0f;
	float Decay = 1.0f;
	float deltaTexCoord = 1;
	float Weight = 1;
	float4 color = txShadows.Sample(samLinear, iTex0);
	
	
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		iTex0 -= deltaTexCoord;

		half3 smple = txShadows.Sample(samLinear, iTex0);

		// Apply sample attenuation scale/decay factors.  
		smple *= illuminationDecay * Weight;
		// Accumulate combined color.  
		color.rgb += smple;
		// Update exponential decay factor.  
		illuminationDecay *= Decay;
	}

	//return color;
	return txDiffuse.Sample(samLinear, iTex0);
}
