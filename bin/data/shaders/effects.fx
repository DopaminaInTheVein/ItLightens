//#include "globals.fx" --> included on data/cubemaps_positions.fx

#include "data/cubemaps_positions.fx"


bool isLeft(float2 a, float2 b, float3 c){
     return ((b.x - a.x)*(c.z - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VSTextureScreen(
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
float4 PSReflections(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	float4 o_color = float4(0,0,0,1);
	//return o_color;
	int3 coords = int3(Pos.xy,0);
	float  z = txDepths.Load(coords).x;
	float3 wPos = getWorldCoords(Pos.xy, z);
	float  glossiness = txGlossiness.Load(coords).r;
	float3 N = txNormal.Load(coords).xyz * 2 - 1;
	float4 specular_color = txSpeculars.Load(coords);
	float4 specular_color1 = txSpecular.Load(coords);
	
	//return glossiness;
	//N = saturate(N);
	//N = normalize(N);
	//return float4(0,0,0,0);
	//return float4(N,1);
	
	//return specular_color;
	//return glossiness.xxxx;
	//return z.xxxx;
	//return float4(wPos,1);
	
	float3 fixedCamPos = CameraWorldPos.zyx;
	float3 fixedWPos = wPos.zyx;


	float3 E_fixed = normalize(fixedCamPos - fixedWPos);

	float3 E_refl = reflect(-E_fixed, N);
	//float3 E_refl = reflect(-E_fixed, float3(0,1,0));
	
	//float3 env = txEnvironment2.Sample(samClampLinear, E_refl).xyz / distance(pos_env2, wPos);
	//return float4(E_refl,1);
	float3 env = GetEnvColor(E_refl, wPos.xyz);
	//o_color.xyz += (env/2) * specular_color1*5;
	
	return specular_color.xxxx;
	//o_color.xyz += (env/2) * specular_color*10;
	o_color.xyz = env.xyz*specular_color*10;
	//return float4(0,0,0,0);
	//return float4(1,1,1,1);
	//o_color.xyz = E_refl.xyz;
	return o_color;
}

//--------------------------------------------------------------------------------------
float4 PSDarken(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	float4 base_color = txDiffuse.Sample(samLinear, iTex0);
	float factor = 0.1f;
	float depth = txDepths.Sample(samLinear, iTex0).r;
	
	float4 rnd = txNoise.Sample(samLinear, iTex0+sin(world_time));
	float int_color = length(base_color.xyz)*factor;
	//float4 o_color = float4(int_color, int_color, int_color, int_color);
	//float4 o_color = float4(base_color.r*factor, base_color.g*factor, base_color.b*factor, base_color.a*factor);
	float4 o_color = base_color*factor;
	o_color.b += o_color.b*0.9f;
	//o_color.a += 1 * fade_black_screen;
	
	//o_color.a = 1 - o_color.a;
	return o_color;
}

float2 SineWave( float2 p ){
    float A = 0.15;
    float w = 10.0 * PI;
    float t = 30.0*PI/180.0;
    float y = sin( w*p.x + t) * A; 
    return float2(p.x, p.y+y);
}


float2 Distort(float2 p)
{
	float BarrelPower = 5;
    float theta  = atan(p);
    float radius = length(p);
    radius = pow(radius, BarrelPower);
    p.x = radius * cos(theta);
    p.y = radius * sin(theta);
    return 0.5 * (p + 1.0);
}     


float4 PSTest(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	float4 base_color = txDiffuse.Sample(samClampLinear, iTex0);
	float4 o_color = base_color;
	
	
	
	//Value between -0.5 and 0.5
	float2 iTex0_norm = iTex0;
	iTex0_norm -= 0.5f;
	iTex0_norm = abs(iTex0_norm);
	iTex0_norm *= iTex0_norm*iTex0_norm*iTex0_norm;
	iTex0_norm *= dream_wave_amplitude;	//8
	
	
	float value_coords = (iTex0_norm.x + iTex0_norm.y)/2;
	
	float border_factor = value_coords;
	//border_factor += sinWt;
		
	
	float4 noise = txNoise.Sample(samLinear, iTex0);
	
	o_color += float4(1,1,1,1)*(border_factor);
	o_color.a = border_factor*4;
	
	//sin between 0-1 with worldtime
	float sinWt = sin(world_time*dream_speed)*0.5 + 1;	//1
	
	//control size waves, lower value = bigger, less waves
	float2 coord_value = iTex0 * dream_waves_size;	//0.5
	
	float rand_1 = sin(coord_value.y*10+world_time*0.3)*sinWt;
	float rand_2 = cos(coord_value.x*10+world_time*0.3)*(1-sinWt);
	//rand += cos(iTex0.x*10+world_time*0.3)*sinWt;
	//rand += cos(iTex0.y*10+world_time*0.3)*(1-sinWt);
	
	//rand/=2;
	
	float freq = 2;
	
	float wave = freq*cos(2*PI*rand_1+coord_value.x*10);
	wave += freq*sin(2*PI*rand_2+coord_value.y*10);
	
	//o_color = base_color;
  
	o_color.a += (wave)*border_factor/2;
	
	//return float4(c.rgb, 1);
	
	float2 offset = iTex0;
	float4 c = float4(0,0,0,0);
	
	//distorsion effect
	float radius = length(offset);
	offset.x += radius * cos(wave)/dream_distorsion_strenght;	//55
	offset.y += radius * sin(wave)/dream_distorsion_strenght;
	c = txDiffuse.Sample(samClampLinear, offset);

	
	//o_color.rgb = float4(0.5,0.5,0.5,0.5)+c; 
	
	//c multiplied by 1 - alpha to extract the alpha influence and not add color
	o_color.rgb = (o_color.a + c*(1-o_color.a));
	o_color.rgb *= color_influence;
	//o_color.rgb -= (wave);
	//o_color.rgb += c;
	
	//return float4(0,0,0,1);
	
	return o_color;
}

