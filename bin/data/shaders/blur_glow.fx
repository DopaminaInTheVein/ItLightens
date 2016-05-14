#include "globals.fx"



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
float dc = txDepths.Sample(samLinear, iTex0).r;
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
		0.120216,
	   0.128764,
	   0.126995,
	   0.164759,
	   0.250985,
	   0.296033,
	   0.339471,
	   0.296033,
	   0.250985,
	   0.164759,
	   0.126995,
	   0.128764,
	   0.120216,
	};

    

    float4 color = {0, 0, 0, 0};
	
	//float3 E = normalize(CameraWorldPos.xyz - Pos);
	//float3 E = normalize(float3(0,0,0) - Pos);
	
    float2 blur = iTex0;
	
	
	//return float4(,1);
	//return float4(Pos,1);
	//return float4(E,1);
	
	float4 color_base = txDiffuse.Sample(samLinear, iTex0.xy) ;
	color = color_base;
	//return color;
	if(direction != 1 && direction != 2){
		return color_base;
	}
	//pixelWidth = strenght_polarize/(xres)/dc;
	float2 pixelWidth = float2(1.0f/xres, 1.0f/yres);
	//pixelWidth = pixelWidth/dc;
	

	float offset_c = 1.0f;

    for (int i = 0; i < 13; i++) 
    {
        blur.x = iTex0.x + Pixels[i] * pixelWidth.x*offset_c;
        color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
    }  
	
	blur.x = iTex0.x;
	
	for (int i = 0; i < 13; i++)
	{
		blur.y = iTex0.y + Pixels[i] * pixelWidth.y*offset_c;
		color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
	}
	
	color = color;
	if(color.w > 1.0f)
		color.w = 1.0f;
		
	//update color intensit y with player life
	color.w = color.w*0.5f+color.w*0.5f*(life_player/150.0f);
	
	if(direction == 2){
		if((color.b - color.r) < 0.0f)
			color = color_base;
	}else if(direction == 1){
		if((color.r - color.b) < 0.0f)
			color = color_base;
	}
	
    return color*offset_c;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSBlur_int(float4 Pos : SV_POSITION
			, float2 iTex0 : TEXCOORD0
			, float3 iWorldPos : TEXCOORD1) : SV_Target
{
float dc = txDepths.Sample(samLinear, iTex0).r;
float2 pixelWidth = float2(1.0f/xres, 1.0f/yres);
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
	
	//pixelWidth = strenght_polarize/(xres)/dc;
	
	float sinw = abs(sin(world_time/3));
	float offset_c = sinw*sinw/2;


    for (int i = 0; i < 13; i++) 
    {
        blur.x = iTex0.x + Pixels[i] * pixelWidth.x*offset_c;
        color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
    }  
	
	blur.x = iTex0.x;
	
	for (int i = 0; i < 13; i++)
	{
		blur.y = iTex0.y + Pixels[i] * pixelWidth.y*offset_c;
		color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
	}
	
	//color *= float4(1.0f,0.3f,0.3f,1.0f);
	
	if (color.r < 0.3 && color.g < 0.3 && color.b < 0.3)
		color.w = 0;
		
    return color*offset_c;
}



