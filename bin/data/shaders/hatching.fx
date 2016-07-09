#include "globals.fx"


Texture2D txSpecular_GL : USE_SHADER_REG(TEXTURE_SLOT_SPECULAR_GL);
Texture2D txShadows : USE_SHADER_REG(TEXTURE_SLOT_SHADOWS);

Texture2D txHatch1 : USE_SHADER_REG(TEXTURE_SLOT_HATCHING);


//Texture2D tx_test1 : USE_SHADER_REG(TEXTURE_SLOT_HATCHING_TEST1);
//Texture2D tx_test2 : USE_SHADER_REG(TEXTURE_SLOT_HATCHING_TEST2);

Texture2D tx_test1 : register(t63);
Texture2D tx_test2 : register(t64);


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
	float specular = txSpeculars.Sample(samLinear, iTex0).r;
	float4 diffuse = txDiffuse.Sample(samLinear, iTex0);
	float4 N = txNormal.Sample(samLinear, iTex0);
	float inv_shadows = 1- txShadows.Sample(samLinear, iTex0).r;
	float base = step(0.1f, specular);
	
	
	float4 val = diffuse*0.3 + specular.xxxx*0.3 + inv_shadows.xxxx*1.0;
	//val*= rim;
	if(val.x > 1)
		val = float4(1,1,1,1);
	val = val*2 -1.6;
	
	//return float4(val.xxx,1);

	float2 noise = txNoise.Sample(samLinear, iTex0).rg;
	
	//return float4(1,1,1,1);
	//float2 pixel_pos = iTex0*frequency_texture;
	float2 pixel_pos = iTex0*3.0f;
	
	float freq_change = frequency_offset;
	float var = sin(world_time*freq_change);
	float offset = step(0.0f, var);
	//offset = 0;
	offset =  offset/2.0f;
	pixel_pos += float2(-offset, offset/2.0f);
	
	//return float4(offset,offset,offset,1.0f);
	//pixel_pos+=noise;
	
	//return float4(specular, specular, specular, 1.0f);
	
	
	
	float diff = saturate(diffuse);
	//float N = normal;


	float4 c = float4(0,0,0,1);
	float step_cmp = 1. / 3.;
	//shading = specular;
	float limit = 0.0f;
	
	
	//txHatch1
	//tests:
	//tx_test1
	c = tx_test1.Sample(samLinear, pixel_pos);
	//return float4(c.a, c.a, c.a, 1.0f);
	
	
	c.r = c.r - 1;
	c.b = c.b - 1;
	c.g = c.g - 1;
	c.a = c.a;
	//return c;
	float alpha = c.a;
	
	

	float intensity = intensity_sketch;

	float color_int = (c.r+c.g+c.b)/3.0f;
	color_int = step(color_int, 0.5f);

	//*diff
	float inv_specular = (1 - specular);
	inv_specular = pow(inv_specular,5);
	diff = 1 - diff;
	float shading = inv_specular*0.2f+diff*0.8f;
	//return float4(specular,specular,specular,1);
	//return float4(inv_specular,inv_specular,inv_specular,1);
	
	//return float4(diff, diff, diff, 1);
	shading = shading*5 - 3;
	//return float4(shading,shading,shading,1);
	//return float4(alpha, alpha, alpha, 1);
	
	
	c.a = alpha*(inv_shadows*inv_shadows)*shading;
	c.a = alpha*val;
	//return float4(inv_shadows, inv_shadows, inv_shadows, 1);
	//c.a = inv_shadows;
	//float LC = 1 - length(c.rgb);
	//c.a *= step(0.2, inv_shadows); //* LC;
	
	//c.a = c.a*2 - 1*c.a;
	//c.a  = 1.0f;
	//return float4(c.a, c.a, c.a, 1.0f);
	//c.a = 0;
	
	c.rgb = diffuse.xyz*0.5f;
	return c;
	
	//return src;
}
