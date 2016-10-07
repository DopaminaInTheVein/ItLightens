
#include "globals.fx"
#include "extern/FXAA.hlsl"

struct FxaaVS_Output {
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


void VS_AA(
  in float4 iPos : POSITION
  , in float2 iTex0 : TEXCOORD0
  , out FxaaVS_Output Output
  )
{
  float4 worldPos = mul(iPos, World);
  Output.Pos = mul(worldPos, ViewProjection);
  Output.Tex = iTex0;

}



float4 PS_AA(FxaaVS_Output Input) : SV_TARGET

{
	FxaaTex InputFXAATex = { samClampLinear, txDiffuse };
	
    float4 c = FxaaPixelShader(
        Input.Tex.xy,							// FxaaFloat2 pos,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsolePosPos,
        InputFXAATex,							// FxaaTex tex,
        InputFXAATex,							// FxaaTex fxaaConsole360TexExpBiasNegOne,
        InputFXAATex,							// FxaaTex fxaaConsole360TexExpBiasNegTwo,
        float2(1/xres,1/yres),					// FxaaFloat2 fxaaQualityRcpFrame,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsoleRcpFrameOpt2,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f),		// FxaaFloat4 fxaaConsole360RcpFrameOpt2,
        0.75f,									// FxaaFloat fxaaQualitySubpix,
        0.166f,									// FxaaFloat fxaaQualityEdgeThreshold,
        0.0833f,								// FxaaFloat fxaaQualityEdgeThresholdMin,
        0.0f,									// FxaaFloat fxaaConsoleEdgeSharpness,
        0.0f,									// FxaaFloat fxaaConsoleEdgeThreshold,
        0.0f,									// FxaaFloat fxaaConsoleEdgeThresholdMin,
        FxaaFloat4(0.0f, 0.0f, 0.0f, 0.0f)		// FxaaFloat fxaaConsole360ConstDir,
    );
	
	c.a = 1;
	return c;
}
