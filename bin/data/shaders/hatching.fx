#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"
#include "constants/ctes_globals.h"


Texture2D txDiffuse   : USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txNormal    : USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txWorldPos  : USE_SHADER_REG(TEXTURE_SLOT_WORLD_POS);
Texture2D txSelfIlum : USE_SHADER_REG(TEXTURE_SLOT_SELFILUM);
Texture2D txDepth : register(t45);

Texture2D txUV : register(t78);
Texture2D txSpecular : register(t79);

Texture2D txHatch1 : register(t80);
Texture2D txHatch2 : register(t81);
Texture2D txHatch3 : register(t82);
Texture2D txHatch4 : register(t83);
Texture2D txHatch5 : register(t84);
Texture2D txHatch6 : register(t85);

Texture2D txEnvironment : USE_SHADER_REG(TEXTURE_SLOT_ENVIRONMENT);

SamplerState samLinear : register(s0);
SamplerState samClampLinear : register(s2);




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

	float dc = txDepth.Sample(samLinear, iTex0).r;
	//float pixel_size = xres*(dc); //adjust line witdh based on depth
	float pixel_size = 1;
	pixel_size *= (xres/yres);
	//pixel_size = 1.0;
	//iTex0
	
	
	//return float4(1,1,1,1);
	float specular = txSpecular.Sample(samLinear, iTex0).r;
	float4 diffuse = txDiffuse.Sample(samLinear, iTex0);
	float4 normal = txNormal.Sample(samLinear, iTex0);
	float4 wPos = txWorldPos.Sample(samLinear, iTex0);
	float2 uvs = txUV.Sample(samLinear, iTex0).xy;
	
	float dist = CameraWorldPos - wPos;
	//dist *= 10;
	
	dist = dist * 10;
	
	//return CameraWorldPos;
	
	//return float4(dist,dist,dist,dist);

	if(dist > 1000)
	 return float4(0,0,0,0);
	//return float4(1,1,1,1);
	float2 pixel_pos = uvs*2;
	//pixel_pos = iTex0*10;
	
	//return float4(specular, specular, specular, 1.0f);
	
	
	
	float diff = saturate(diffuse);
	float N = normal;

	float rim = max( 0., abs( dot( N, -wPos.xyz ) ) );
	//if( invertRim == 1 ) rim = 1. - rim;
	//rim = 1 -rim;
	rim *= 1.0f;
	diff *= 2.0f;
	specular *= 10.0f;
	float shading =  diff + rim + specular;
	//shading = shading;
	//shading = 0;
	//return float4(shading, shading, shading, 1.0f);
	//if( solidRender == 1 ) return vec4( shading );

	float4 c = float4(0,0,0,1);
	float step_cmp = 1. / 6.;
	if (shading <= step_cmp) {
		//return float4(1,0,0,1);
		c = lerp(txHatch6.Sample(samLinear, pixel_pos), txHatch5.Sample(samLinear, pixel_pos), 6.0f * shading);
	}
	if (shading > step_cmp && shading <= 2. * step_cmp) {
		//return float4(0,1,0,1);
		c = lerp(txHatch5.Sample(samLinear, pixel_pos), txHatch4.Sample(samLinear, pixel_pos) , 6.0f * (shading - step_cmp));
	}
	if (shading > 2. * step_cmp && shading <= 3. * step_cmp) {
		//return float4(0,0,1,1);
		c = lerp(txHatch4.Sample(samLinear, pixel_pos), txHatch3.Sample(samLinear, pixel_pos), 6.0f * (shading - 2. * step_cmp));
	}
	if (shading > 3. * step_cmp && shading <= 4. * step_cmp) {
		//return float4(1,1,0,1);
		c = lerp(txHatch3.Sample(samLinear, pixel_pos), txHatch2.Sample(samLinear, pixel_pos), 6.0f * (shading - 3. * step_cmp));
	}
	if (shading > 4. * step_cmp && shading <= 5. * step_cmp) {
		//return float4(1,0,1,1);
		c = lerp(txHatch2.Sample(samLinear, pixel_pos), txHatch1.Sample(samLinear, pixel_pos), 6.0f * (shading - 4. * step_cmp));
	}
	if (shading > 5. * step_cmp) {
		//return float4(0,1,1,1);
		c = lerp(txHatch1.Sample(samLinear, pixel_pos), float4(1.0f,1.0f,1.0f,1.0f), 6.0f * (shading - 5. * step_cmp));
	}
	
	/*if((iTex0.y % 2) == 0)
		return float4(0,0,0,0);*/
	
	//return float4(1,1,1,1);
	//c = 1 - c;
	//return float4(1,1,1,1);
	//return c;
	c.a = 1.0f;
	float w = c.r;
	w = step(w, 0.60f);
	//w = 1 -w;
	w *= 0.2f;
	return float4(w,w,w,w);
	//c = float4(c.r,c.r,c.r,c.r);
	
	//c = step(c, 0.5f);
	

	//float4 src = lerp(lerp(diffuse, float4(1.0,1.0,1.0,1.0), c.r), c, .5);
	//src = lerp(lerp(diffuse, float4(1.0,1.0,1.0,1.0), c.b), c, .5);
	//src = lerp(lerp(diffuse, float4(1.0,1.0,1.0,1.0), c.b), c, .5);
	//c = 1. - ( 1. - src ) * ( 1. - dc );
	//c = float4( min( src.r, dc ), min( src.g, dc ), min( src.b, dc ), 1. );

	//c = float4( iTex0.x / xres, iTex0.y / yres, 0., 1. );
	return c;
	
	//return src;
}
