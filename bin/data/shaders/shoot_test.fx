#include "globals.fx"



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
float4 PSTestShoot_w(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	float dc = txDepths.Sample(samLinear, iTex0).r;
	//float background_color = txFinalImage.Sample(samLinear, iTex0);
    dc = 1.0f;
	float4 color = txDiffuse.Sample(samLinear, iTex0);
	float4 colorShoot = float4(1.0f,0.5f,0.5f,1.0f);
	//float pixel_weight = 1/(xres)/dc;
	float pixel_size = 1/yres;
	
	float max_range = 6.0f;
	float max_range_solid = 2.0f;
	
	float4 final_color = float4(0,0,0,0);
	float weight_trans = 0.3f;
	float weight_solid = 0.8f;
	
	//check down
	for(int i = 0; i< max_range; i++){
		float4 new_color = txDiffuse.Sample(samLinear, iTex0+(i*pixel_size));
		if(new_color.r > 0.5f){
			if(i<max_range_solid){
				final_color += colorShoot*weight_solid;
			}else{
				final_color += colorShoot*weight_trans;
			}
		}
	}
	
	//check up
	for(i = 0; i>-max_range; i--){
		float4 new_color = txDiffuse.Sample(samLinear, iTex0-(i*pixel_size));
		if(new_color.r > 0.5f){
			if(i>-max_range_solid){
				final_color += colorShoot*weight_solid;
			}else{
				final_color += colorShoot*weight_trans;
			}
		}
	}
	
	//float stime = sin(world_time);
	//float ctime = cos(world_time);
	
	//float time_fx = stime*ctime;
	
	//return color;
	
	return float4(1,1,1,1);
	//color.w = 0.5f;
	return final_color;
}



float4 PSTestShoot(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	
	float dc = abs(txDepths.Sample(samLinear, iTex0).r);
	
	float4 color = txDiffuse.Sample(samClampLinear, iTex0);
	//return color;
	if(color.r > 0.1f)
	 return float4(1,1,1,1);
	float4 colorShoot = float4(1.0f,0.3f,0.3f,1.0f);
	float4 colorShootW = float4(1.0f,1.0f,1.0f,1.0f);
	
	float stime = sin(world_time);
	float ctime = cos(world_time);
	float offset = abs(4/dc);
	
	float rnd = txNoise.Sample(samLinear, iTex0).r;
	float rand = sin(rnd);
		
	offset = 1.0f;
		
	float2 pixel_size = float2(1/(xres) ,1/(yres));
	pixel_size = pixel_size;
	
	//offset = rand*10.0f;
	offset = 1.0f;
	float max_range = 12.0f+offset;
	float max_range_solid = 1.0f;
	float strength = 8.0f;
	
	float4 final_color = float4(0,0,0,0);
	float weight_solid = 1.0f;
	
	float2 posTex = iTex0;
	
	//check down
	for(int i = 0; i< max_range; i++){
		posTex.x = iTex0.x+(i*pixel_size.x);
		float4 new_color = txDiffuse.Sample(samClampLinear, posTex);
		if(new_color.r > 0.1f){
			if(i<max_range_solid){
				final_color = colorShootW*weight_solid;
				//final_color.w = 1;
			}else{
				float c = abs((i - max_range_solid)/strength);
				float4 test_color = colorShoot*(weight_solid)/c;
				if(test_color.w > final_color.w)
					final_color = test_color;
				//final_color.w = 1;
			}
		}
	}
	posTex = iTex0;
	//check up
	for(int i = 0; i>-max_range; i--){
		posTex.x = iTex0.x+(i*pixel_size.x);
		float4 new_color = txDiffuse.Sample(samClampLinear, posTex);
		if(new_color.r > 0.1f){
			if(i>-max_range_solid){
				final_color = colorShootW*weight_solid;
				//final_color.w = 1;
			}else{
				float c = (abs(i) + max_range_solid)/strength;
				float4 test_color = colorShoot*weight_solid/c;
				if(test_color.w > final_color.w)
					final_color = test_color;
				//final_color.w = 1;
			}
		}
	}
	
	posTex = iTex0;
	
	for(int i = 0; i< max_range; i++){
		posTex.y = iTex0.y+(i*pixel_size.y);
		float4 new_color = txDiffuse.Sample(samClampLinear, posTex);
		if(new_color.r > 0.1f){
			if(i<max_range_solid){
				final_color = colorShootW*weight_solid;
				//final_color.w = 1;
			}else{
				float c = abs((i - max_range_solid)/strength);
				float4 test_color = colorShoot*(weight_solid)/c;
				if(test_color.w > final_color.w)
					final_color = test_color;
				//final_color.w = 1;
			}
		}
	}
	posTex = iTex0;
	//check up
	for(int i = 0; i>-max_range; i--){
		posTex.y = iTex0.y+(i*pixel_size.y);
		float4 new_color = txDiffuse.Sample(samClampLinear, posTex);
		if(new_color.r > 0.1f){
			if(i>-max_range_solid){
				final_color = colorShootW*weight_solid;
				//final_color.w = 1;
			}else{
				float c = (abs(i) + max_range_solid)/strength;
				float4 test_color = colorShoot*weight_solid/c;
				if(test_color.w > final_color.w)
					final_color = test_color;
				//final_color.w = 1;
			}
		}
	}
	
	
	
	stime*=stime*rand/dc;
	ctime*=ctime*rand/dc;
	
	float time_fx = stime*ctime;
	
	float4 mask_value = txNoise.Sample(samLinear, float2(stime,ctime));
	final_color *= mask_value;
	//return color;
	//final_color.w = 1;
	//return float4(1,1,1,1);
	//color.w = 0.5f;
	return final_color;
}