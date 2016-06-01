#include "globals.fx"


Texture2D txSpecular : register(t79);
Texture2D txInvShadows : register(t78);

Texture2D txHatch1 : register(t80);
Texture2D txHatch2 : register(t81);
Texture2D txHatch3 : register(t82);
Texture2D txHatch4 : register(t83);
Texture2D txHatch5 : register(t84);
Texture2D txHatch6 : register(t85);


Texture2D txHatch_test8 : register(t86);
Texture2D txHatch_test9 : register(t87);

Texture2D txHatch_test : register(t88);
Texture2D txHatch_test2 : register(t89);
Texture2D txHatch_test3 : register(t90);
Texture2D txHatch_test4 : register(t91);
Texture2D txHatch_test5 : register(t92);
Texture2D texture_hatch : register(t93);
Texture2D txHatch_test7 : register(t94);



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VSCrossHatching(
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
float4 PSCrossHatching(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	int3 ss_load_coords = uint3(Pos.xy, 0);
	float  z = txDepths.Load(ss_load_coords).x;
	float3 wPos = getWorldCoords(Pos.xy, z);

	float2 pixel_size = float2(1.0f/xres, 1.0f/yres);
	//pixel_size *= (xres/yres);
	//pixel_size = 1.0;
	//iTex0
	
	
	//return float4(1,1,1,1);
	float specular = txSpecular.Sample(samLinear, iTex0).r;
	float4 diffuse = txDiffuse.Sample(samLinear, iTex0);
	float4 normal = txNormal.Sample(samLinear, iTex0);
	float inv_shadows = 1- txInvShadows.Sample(samLinear, iTex0).r;
	float base = step(0.1f, specular);
	
	//return float4(specular, specular, specular, 1.0f);
	//if(base == 1.0f)
		//return float4 (0.0f,0.0f,0.0f, 0.0f);
	
	//return CameraWorldPos;
	
	//return float4(dist,dist,dist,dist);

	//if(dist > 1000)
	// return float4(0,0,0,0);
	
	float2 noise = txNoise.Sample(samLinear, iTex0).rg;
	
	//return float4(1,1,1,1);
	float2 pixel_pos = iTex0*frequency_texture;
	//float2 pixel_pos = iTex0*1.0f;
	
	float freq_change = frequency_offset;
	float var = sin(world_time*freq_change);
	float offset = step(0.0f, var);
	pixel_pos += offset/2.0f;
	
	//return float4(offset,offset,offset,1.0f);
	//pixel_pos+=noise;
	
	//return float4(specular, specular, specular, 1.0f);
	
	
	
	float diff = saturate(diffuse);
	float N = normal;

	float rim = max( 0., abs( dot( N, -wPos.xyz ) ) );
	//if( invertRim == 1 ) rim = 1. - rim;
	//rim = 1 -rim;
	rim *= rim_strenght;
	diff *= diffuse_strenght;
	diff = 1.0f;
	specular *= specular_strenght;
	float shading =  diff + rim + specular;
	//shading = shading;
	//shading = 0;
	//return float4(shading, shading, shading, 1.0f);

	float4 c = float4(0,0,0,1);
	float step_cmp = 1. / 3.;
	
	float limit = 0.0f;
	
	
	//if (shading <= step_cmp) {
	if (inv_shadows >= limit) {
		//return float4(1,1,0,1);
		//c = lerp(txHatch_test6.Sample(samLinear, pixel_pos), txHatch_test6.Sample(samLinear, pixel_pos), 3.0f * (shading - 3. * step_cmp));
		//c = txHatch_test6.Sample(samLinear, pixel_pos);
		c = lerp(txHatch1.Sample(samLinear, pixel_pos), float4(1.0f,1.0f,1.0f,1.0f), 3.0f * (shading - 2. * inv_shadows));
	}
	/*if (shading > 3. * step_cmp && shading <= 2. * step_cmp) {
		//return float4(1,0,1,1);
		c = lerp(txHatch_test6.Sample(samLinear, pixel_pos), txHatch_test6.Sample(samLinear, pixel_pos), 3.0f * (shading - 4. * step_cmp));
	}*/
	//if (shading > 1. * step_cmp) {
	if (inv_shadows < limit) {
		//return float4(0,1,1,1);
		c = lerp(txHatch1.Sample(samLinear, pixel_pos), float4(1.0f,1.0f,1.0f,1.0f), 3.0f * (shading - 2. * inv_shadows));
		//c = txHatch_test6.Sample(samLinear, pixel_pos);
	}
	
	/*if((iTex0.y % 2) == 0)
		return float4(0,0,0,0);*/
	
	//return float4(1,1,1,1);
	//c = 1 - c;
	//return float4(1,1,1,1);
	//return c;

	float intensity = intensity_sketch;

	float color_int = (c.r+c.g+c.b)/3.0f;
	color_int = step(color_int, 0.5f);
	//color_int = 1- color_int;
	//return float4(color_int, color_int, color_int, 1.0f);
	//intensity = step(0.5f, intensity);
	float alpha = color_int*intensity;
	
	/*if(c.r > 0.3f){
		return float4(0.0f,0.0f,0.0f,0.0f);
	}*/
	
	//alpha = 1.0f;
	//c.a = alpha;
	//c = 1-c;
	
	
	//return float4(inv_shadows, inv_shadows, inv_shadows, 1.0f);
	c.a = alpha;
	c.a = alpha*(inv_shadows);
	//c.a  = 1.0f;
	c.rgb = float3(0,0,0);		//lines black
	//float w = c.r;
	//w = step(w, 0.60f);
	//w = 1 -w;
	//w *= 0.2f;
	//return float4(w,w,w,w);
	//c = float4(c.r,c.r,c.r,c.r);
	
	//c = step(c, 0.5f);
	
	

	return c;
	
	//return src;
}
