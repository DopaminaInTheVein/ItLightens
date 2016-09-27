#include "globals.fx"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VSDoF(
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
  float4 c0  = txDiffuse.Sample(samClampLinear, iTex0.xy);
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

  /*float depth = txDepths.Sample(samLinear, iTex0.xy);
  
  //float factor = (1 - depth);
  
  float factor = depth*10;
  
  float2 offset = float2(factor/xres, factor/yres);
  //factor = 1;
	//factor = 0;
	
	float speed =1;
	
	float sinWt = cos(world_time*speed+length(iTex0));

	//sinWt *= 10/xres;
	
	float2 dist = iTex0;
	float radius = length(iTex0);
	dist.x += radius * cos(iTex0)/55;	
	dist.y += radius * sin(iTex0)/55;
	float4 c = txDiffuse.Sample(samClampLinear, dist);
	
	offset += sinWt;*/
	
  // 7 tap blur controlled by the vs
  float4 cp3 = txDiffuse.Sample(samClampLinear, iTex3.zw);
  float4 cp2 = txDiffuse.Sample(samClampLinear, iTex2.zw );
  float4 cp1 = txDiffuse.Sample(samClampLinear, iTex1.zw );
  float4 c0  = txDiffuse.Sample(samClampLinear, iTex0.xy);
  float4 cn1 = txDiffuse.Sample(samClampLinear, iTex1.xy );
  float4 cn2 = txDiffuse.Sample(samClampLinear, iTex2.xy );
  float4 cn3 = txDiffuse.Sample(samClampLinear, iTex3.xy );

  float4 cfinal =
    c0 * blur_w.x
    + (cp1 + cn1) * blur_w.y
    + (cp2 + cn2) * blur_w.z
    + (cp3 + cn3) * blur_w.w
    ;

	cfinal *= float4(0.1,0.1f,0.5f,1);
	//return float4(depth, depth, depth, 1.0f);
	
	return cfinal;//*0.8+c*0.2;

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

float factor_depth = 4;
  float base_depth = txDepths.Sample(samLinear, iTex0.xy).r;
  
  
  float depth = base_depth * factor_depth;
  //float factor = (1 - depth);
  
  //make exponencial curve;
 depth = pow(depth,2);
  
  //depth -= depth;
  if(depth < 0)
	depth = 0;
  
  
  
  float depth_focus = txDepths.Sample(samLinear, float2(0.5,0.6)).r;
  //return float4(depth_focus.rrr,1);
  
  /*//test_center
  if(iTex0.y <= 0.405f && iTex0.y >= 0.395f){
	if(iTex0.x <= 0.505f && iTex0.x >= 0.495f)
		return float4(0,0,1,1);
  }
  
  //auto focus
  float barrier_focus = 0.05;
  //float4 cfinal;
  float differences_depths = abs(base_depth - depth_focus);
  //return float4(differences_depths.xxx,1);
  
  if( differences_depths > barrier_focus){
  
	//depth focus should be more soft than depth of field,
	//so we divide factor_depth by 2
	depth += 1;
	//return float4(depth.xxx,1)*float4(0,1,0,1);
  }else{
	//return float4(depth.xxx, 1)*float4(1,0,0,1);
  }*/
  
  float factor = depth;
  float2 offset = float2(factor/xres, factor/yres);
  //factor = 1;
  
  //return float4(offset.yyy, 1);
	//factor = 0;

  // 7 tap blur controlled by the vs
  float4 cp3 = txDiffuse.Sample(samClampLinear, iTex0.xy - offset);
  float4 cp2 = txDiffuse.Sample(samClampLinear, iTex0.xy - offset);
  float4 cp1 = txDiffuse.Sample(samClampLinear, iTex0.xy - offset);
  float4 c0  = txDiffuse.Sample(samClampLinear, iTex0.xy);
  float4 cn1 = txDiffuse.Sample(samClampLinear, iTex0.xy + offset);
  float4 cn2 = txDiffuse.Sample(samClampLinear, iTex0.xy + offset);
  float4 cn3 = txDiffuse.Sample(samClampLinear, iTex0.xy + offset);

  float4 cfinal =
    c0 * blur_w.x
    + (cp1 + cn1) * blur_w.y
    + (cp2 + cn2) * blur_w.z
    + (cp3 + cn3) * blur_w.w
    ;
	
	//return float4(depth, depth, depth, 1);

	return float4(1, 1, 1, 0.0f);
	
	return float4(cfinal.rgb,depth*factor_depth);

}

static float2 offs[7] = {
   float2( 0.0,  0.0), //Center       0
   float2(-1, -1), //Top Left     1
   float2( 1,  1), //Bottom Right 5
   float2( 1, -1), //Top Right    3
   float2(-1,  1), //Bottom Left  7
   float2(-1,  0.0),  //Left         8
   float2( 0.0, -1) //Top          2
}; 

float GetEdgeValue(float2 uv){

	  //float2 e_barrier = float2(0.5f,edge_lines_detection);  // x=norm, y=depth
	float2 e_barrier = float2(0.5f,0.00005f);
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
	 dd.x = txDepths.Sample(samClampLinear, uv + offs[1]*pixel_size).r +

			 txDepths.Sample(samClampLinear, uv + offs[2]*pixel_size).r;

	 dd.y = txDepths.Sample(samClampLinear, uv + offs[3]*pixel_size).r+

			txDepths.Sample(samClampLinear, uv + offs[4]*pixel_size).r;

	 dd.z = txDepths.Sample(samClampLinear, uv + offs[5]*pixel_size).r +

			txDepths.Sample(samClampLinear, uv + tc5r*pixel_size).r;

	 dd.w = txDepths.Sample(samClampLinear,uv +  offs[6]*pixel_size).r+

			txDepths.Sample(samClampLinear,uv +  tc6r*pixel_size).r;

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
	 
	 float3 N_pixel = txNormal.Sample(samClampLinear,uv).xyz * 2 - 1;
	
	float3 normals[6];
	float dots = 0;
	for(int i =1; i<7;i++ ){
		int pos = i-1;
		 normals[pos] = txNormal.Sample(samClampLinear,uv + offs[i]*pixel_size).xyz * 2 - 1;
		 dots += dot(N_pixel, normals[pos]);
	}
	 float w = (1 - de) * e_kernel.x; 
	//return float4(1,0,1,1);
	//w = step(0.75f, w);
	//return dots/30+w;
	return w;
}

//--------------------------------------------------------------------------------------
float4 PSAntiAliasing(
  in float4 iPosition : SV_Position
  , in float2 iTex0 : TEXCOORD0
  , in float4 iTex1 : TEXCOORD1
  , in float4 iTex2 : TEXCOORD2
  , in float4 iTex3 : TEXCOORD3
) : SV_Target
{

  float depth = txDepths.Sample(samLinear, iTex0.xy);
  
  
  //return float4(depth.rrr,1);
  
  float edge = GetEdgeValue(iTex0);
  if(edge == 0){
	return float4(0,0,0,0);
  }
  
  float factor = edge;//*10;
 
  
  float2 offset = float2(factor/xres, factor/yres);
  //factor = 1;
	//factor = 0;
	
		return float4(edge,edge,edge,1);

  // 7 tap blur controlled by the vs
  float4 cp3 = txDiffuse.Sample(samClampLinear, iTex0.xy - offset);
  float4 cp2 = txDiffuse.Sample(samClampLinear, iTex0.xy - offset);
  float4 cp1 = txDiffuse.Sample(samClampLinear, iTex0.xy - offset);
  float4 c0  = txDiffuse.Sample(samClampLinear, iTex0.xy);
  float4 cn1 = txDiffuse.Sample(samClampLinear, iTex0.xy + offset);
  float4 cn2 = txDiffuse.Sample(samClampLinear, iTex0.xy + offset);
  float4 cn3 = txDiffuse.Sample(samClampLinear, iTex0.xy + offset);

  float4 cfinal =
    c0 * blur_w.x
    + (cp1 + cn1) * blur_w.y
    + (cp2 + cn2) * blur_w.z
    + (cp3 + cn3) * blur_w.w
    ;
	
	//return float4(depth, depth, depth, 1);

	//return float4(1, 1, 1, 1.0f);
	
	return float4(cfinal.rgb,1);

}



