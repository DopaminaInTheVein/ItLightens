#include "globals.fx"

//--------------------------------------------------------------------------------------
// Null vertex/pixel shader
//--------------------------------------------------------------------------------------
void VSNull(
	in float4 iPos : POSITION
	, out float4 oPos : SV_POSITION
	)
{
	float4 worldPos = mul(iPos, World);
	oPos = mul(worldPos, ViewProjection);
}

//--------------------------------------------------------------------------------------
float4 PSNull(float4 Pos : SV_POSITION
	) : SV_Target
{
	return float4(0,0,0,0);
}

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
	oPos = mul(worldPos, ViewProjection);
	oColor = iColor * obj_color;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(float4 Pos : SV_POSITION
	, float4 iColor : COLOR
	) : SV_Target
{
	return iColor;
}

//--------------------------------------------------------------------------------------
void VS_N_UV(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL
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
// Vertex Shader for pos + uv vertexs types
//--------------------------------------------------------------------------------------
void VSTextured(
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
float4 PSPostProcess(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	float4 color = txDiffuse.Sample(samClampLinear, iTex0);
	//if (color.w == 0)
		//discard;
	return color;
}

//--------------------------------------------------------------------------------------
float4 PSTextured(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	return txDiffuse.Sample(samLinear, iTex0);
}

void PSMultiple(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	, out float4 output1 : SV_Target0
	, out float4 output2 : SV_Target1
	)
{
	output1 = txDiffuse.Sample(samLinear, iTex0);
	
	output2 = output1;
}

//--------------------------------------------------------------------------------------
void PSAddAmbient(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	, out float4 output : SV_Target
	) 
{

	float4 diff = txDiffuse.Sample(samClampLinear, iTex0);
	float4 lights = txEnvironment.Sample(samLinear, iTex0);
	float4 normals = txNormal.Sample(samLinear, iTex0); 
	float4 selfIlum = txSelfIlum.Sample(samLinear, iTex0);
	
	
	output = diff*0.2f + lights*0.8f;
	//output = float4(0.2f,0.2f, 0.2f, 1.0f)*normals*0.2f + lights*0.8f;
	
	//output = lights;
	output.a = diff.a;
	
	//output = float4(0,0,0,1);
	//output.rgb += selfIlum.rgb;
}

//--------------------------------------------------------------------------------------
struct VS_TEXTURED_OUTPUT
{
	float4 Pos    : POSITION;
	float2 UV     : TEXCOORD0;
	float4 wPos   : TEXCOORD1;
};

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


static float width_line = 1./5.;

float4 PSBlur(float4 Pos : SV_POSITION
			, float2 iTex0 : TEXCOORD0
			, float3 iWorldPos : TEXCOORD1) : SV_Target
{
float dc = txDepths.Sample(samLinear, iTex0).r;
float pixelWidth = 1.0f;
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
	
	
	pixelWidth = width_line/(xres)/dc;
	
	float sinw = abs(sin(world_time/3));
	float offset_c = sinw*sinw/2;


    for (int i = 0; i < 13; i++) 
    {
        blur.x = iTex0.x + Pixels[i] * pixelWidth*offset_c;
        color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
    }  
	
	blur.x = iTex0.x;
	
	for (int i = 0; i < 13; i++)
	{
		blur.y = iTex0.y + Pixels[i] * pixelWidth*offset_c;
		color += txDiffuse.Sample(samLinear, blur.xy) * BlurWeights[i];
	}
	
	//color *= float4(1.0f,0.3f,0.3f,1.0f);
	
	if (color.r < 0.3 && color.g < 0.3 && color.b < 0.3)
		color.w = 0;
		
    return color*offset_c;
}



//--------------------------------------------------------------



float4 PShader2(float4 Pos : SV_POSITION
	, float2 uv : TEXCOORD0) : SV_TARGET

{

float i = 1.0f;

float2 offs[7] = {
   float2( 0.0,  0.0), //Center       0
   float2(-i, -i), //Top Left     1
   float2( i,  i), //Bottom Right 5
   float2( i, -i), //Top Right    3
   float2(-i,  i), //Bottom Left  7
   float2(-i,  0.0),  //Left         8
   float2( 0.0, -i) //Top          2
}; 


//float2 e_barrier = float2(0.5f,edge_lines_detection);  // x=norm, y=depth
float2 e_barrier = float2(0.5f,0.0001f);
float2 pixel_size = float2(1.0f/xres,1.0f/yres);	//to adjust line witdh
//float pixel_size = 1.0f;
float2 e_weights= float2(1,1);  // x=norm, y=depth

float2 e_kernel = float2(1,1);   // x=norm, y=depth

 
float dc = txDepths.Sample(samLinear, uv).r;
pixel_size = pixel_size/(dc*CameraZFar); //adjust line witdh based on depth


float size_line = 1.0f;
pixel_size *= size_line;


float2 tc5r = -offs[5];

float2 tc6r = -offs[6];


 
//return float4(dc,dc,dc,1);
//return float4(1,1,1,1);
 float4 dd;

 //float2 test = float2(10,10);
 //return float4(uv.yyyyy);
 dd.x = txDepths.Sample(samLinear, uv + offs[1]*pixel_size).r +

         txDepths.Sample(samLinear, uv + offs[2]*pixel_size).r;

 dd.y = txDepths.Sample(samLinear, uv + offs[3]*pixel_size).r+

        txDepths.Sample(samLinear, uv + offs[4]*pixel_size).r;

 dd.z = txDepths.Sample(samLinear, uv + offs[5]*pixel_size).r +

        txDepths.Sample(samLinear, uv + tc5r*pixel_size).r;

 dd.w = txDepths.Sample(samLinear,uv +  offs[6]*pixel_size).r+

        txDepths.Sample(samLinear,uv +  tc6r*pixel_size).r;

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
w = step(0.75f, w);
//w=global_color*w;
float4 final_color = w*global_color;
return final_color;


}

float4 PSGlobalColor(float4 Pos : SV_POSITION
	, float2 uv : TEXCOORD0) : SV_TARGET

{
	return global_color;
}

float4 PSWhiteColor(float4 Pos : SV_POSITION
	, float2 uv : TEXCOORD0) : SV_TARGET

{
	return float4(1,1,1,1);
}

float4 PSRedColor(float4 Pos : SV_POSITION
	, float2 uv : TEXCOORD0) : SV_TARGET

{
	return float4(1,0,0,1);
}

float4 PSGreenColor(float4 Pos : SV_POSITION
	, float2 uv : TEXCOORD0) : SV_TARGET

{
	return float4(0,1,0,1);
}


float4 PSOutlineStencil(float4 Pos : SV_POSITION
	, float2 uv : TEXCOORD0) : SV_TARGET

{
	return float4(1,1,1,1);
}
