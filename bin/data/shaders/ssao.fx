#include "globals.fx"

static int n = ssao_iterations;
static float occ_offset = 0.001 * ssao_intensity;

float getOcc(float3 N, float3 w, float3 N_sample, float R, float depth, float depth_sample, float3 N_acum)
{
   float Approx = 0.0f;
 
	float3 wN = normalize(w); 
	
	float dotN = dot(N, N_sample);
   float dotNacum =  dot(N_sample, N_acum);
   
   float3 diff = dot(wN, N);
   
   float e = 1 - dotN;
	
   //depth differences
   float z_diff = depth-depth_sample;
   
   if(z_diff > occ_offset)	//barrier for depth, object too far away
	return 0;
   else
	return (e*diff+z_diff)/n;
   
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

/*
//--------------------------------------------------------------------------------------
//####### OLD VERSION #######

void PS(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	, out float4 o_color : SV_Target
	//, out float4 o_shadows : SV_Target0
	)
{
   
   int3 ss_load_coords = uint3(Pos.xy, 0);
  
  // Recuperar la posicion de mundo para ese pixel
  float  z = txDepths.Load(ss_load_coords).x;
  //float z_scaled = z*2.0f;
  float3 position = getWorldCoords(Pos.xy, z);
  //o_color = float4(z,z,z, 1);
   float3 normal = txNormal.Load(ss_load_coords).xyz * 2 - 1;
 //return float4(normal.x, normal.y, normal.z, 1);
	//o_color = float4(normal,1);
   //z /= 5;
   
   //Range of influence
   float R = 3.50f;
 
   //The point at which the fall-off function reaches its peak of 1
   float c = 0.1f * R;
 
   //Number of samples used
  // int n = 20;
 //z-=0.1;
   //Calculate the sampling distance and step size
   
   //z = distance(CameraWorldPos.xyz, position);
   
   float sArea = R / (z);
   float sStep = sArea / n;
 
 //return sArea/100;
 //return sStep/100;
   //Starting angle is read in from a noise texture for a randomization factor
   float4 random = txNoise.Sample(samLinear, iTex0);
   float angle = random.r+random.g;
   angle /= (random.b);
   //angle = 1;
   angle *= 2.0f * PI;
   
   //return angle/100;
 
   //Calculates how many steps will need to be taken to go full circle
   float angleStep = 2.0f * PI / n;
 //return  angleStep;
   //Initialize the Monte-Carlo sum value
   float occ = 0.0f;
  float3 n_acum =normal;
   //Iterate through all samples
   for(int i = 0; i < n; ++i)
   {
      //Change the sampling coordinates for the next sample
      float2 pixelOffset = float2(sStep * cos(angle), sStep * sin(angle));
	  
	  //float2 pixel_sample = Pos + pixelOffset;
	 
	  pixelOffset *= float2(1/xres, 1/yres);
 
	 float2 pixel_sample = iTex0 + pixelOffset;
   
  // Recuperar la posicion de mundo para ese pixel
  //float  z_sample = txDepths.Load(ss_load_coords_sample).x;
  float z_sample = txDepths.Sample(samClampLinear, pixel_sample).x;
  float3 n_sample = txNormal.Sample(samClampLinear, pixel_sample).xyz * 2 - 1;

   
 
  //return z_sample;
  float3 pixelPosition_sample = getWorldCoords(pixel_sample.xy, z_sample);
  //return float4(pixelPosition_sample,1);
 
      //Vector from the current position to the position of the sampled pixel
      float3 w = pixelPosition_sample - position;
	
      //get occ value for pixel sample
      occ += getOcc(normal, w, n_sample, R, z, z_sample, n_acum);
	//return monteCarlo;
      //Offsets the angle for the next sampling
      angle += angleStep;
	  n_acum = (n_sample+n_acum)/2.f;
      n_acum = normalize(n_acum);
   }
   
   //return occ;
   //A factor that will multiply the final occ sum
   float factor = ((2.0f * PI * c));
 
   //Calculating the final occlusion value
   float A = (factor * occ);
   
	//o_shadows = float4(0,0,0,1);
   //o_color = float4(A, A, A, 1);
	o_color = occ*c;
	//o_color.a = 1 - o_color.r;
	o_color/=1.0f;
	o_color.a = 1;
	
	//o_color = float4(n_acum,1);
	//we maintan only the dark spots with high alpha value
/*	o_shadows = A;
	o_shadows.a = 1 - o_shadows.r;
	o_shadows.rgb *= 10;*/
	//o_shadows = 0;
//} 


static float g_sample_rad = ssao_sample_rad;
static float g_intensity = ssao_test_intensity;
static float g_scale = ssao_scale;
static float g_bias = ssao_bias;

float3 getPosition(in float2 uv)
{
	int3 ss_load_coords = int3(uv,0);
	float  z = txDepths.Load(ss_load_coords).x;
	float3 position = getWorldCoords(uv.xy, z);
	return position;
	return mul(position, InvViewProjection);
}

float3 getNormal(in float2 uv)
{
	int3 ss_load_coords = int3(uv,0);
	return txNormal.Load(ss_load_coords).xyz * 2 - 1;
}

float2 getRandom(in float2 uv)
{
	float4 rnd = txNoise.Sample(samLinear, uv);
	return rnd.xy;
	return (rnd.xy/rnd.wz);
}

float doAmbientOcclusion(in float2 tcoord,in float2 uv, in float3 p, in float3 cnorm, float z)
{
	float2 pos_offset = tcoord + uv;
	float3 diff = getPosition(tcoord + uv) - p;
	
	int3 ss_load_coords = int3(pos_offset.xy,0);
	float  new_z = txDepths.Load(ss_load_coords).x;
	
	const float3 v = normalize(diff);
	float d = length(diff)*g_scale ;
	
	//return v;
	return max(0.0,dot(cnorm,v)-g_bias)*(1.0/(1.0+d))*g_intensity/(1+abs(new_z-z)*7000);
}

float4 PSInv(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	//, out float4 o_shadows : SV_Target0
	): SV_Target
{

	const float2 vec[4] = {float2(1,0),float2(-1,0),
				float2(0,1),float2(0,-1)};

	int3 ss_load_coords = int3(Pos.xy,0);
	float  z = txDepths.Load(ss_load_coords).x;
	
	float3 p = getPosition(Pos.xy);
	float3 n = getNormal(Pos.xy);
	float2 rand = getRandom(Pos.xy);

	float ao = 0.0f;
	float rad = g_sample_rad/z;
	
	//return float4(rad.xxx/500,1);
	
	//return doAmbientOcclusion(Pos.xy, Pos.xy, p, n);

//o_color = float4(rand.xxx,1);
	int iterations = ssao_iterations;
	for (int j = 0; j < iterations; ++j)
	{
		  float2 coord1 = reflect(vec[j],rand)*rad;
		  float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707,
					  coord1.x*0.707 + coord1.y*0.707);
		  
		  ao += doAmbientOcclusion(Pos.xy,coord1*0.25, p, n, z);
		  ao += doAmbientOcclusion(Pos.xy,coord2*0.5, p, n, z);
		  ao += doAmbientOcclusion(Pos.xy,coord1*0.75, p, n, z);
		  ao += doAmbientOcclusion(Pos.xy,coord2, p, n, z);
	}
	ao/=(float)iterations*4.0;
	
	float4 o_color = float4(ao, ao, ao, ao);
	//o_color = float4(1,1,1,1);
	o_color.a = 1;
	return 1-o_color;
}

float4 PS(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	//, out float4 o_shadows : SV_Target0
	): SV_Target
{

	const float2 vec[4] = {float2(1,0),float2(-1,0),
				float2(0,1),float2(0,-1)};

	int3 ss_load_coords = int3(Pos.xy,0);
	float  z = txDepths.Load(ss_load_coords).x;
	
	float3 p = getPosition(Pos.xy);
	float3 n = getNormal(Pos.xy);
	float2 rand = getRandom(Pos.xy);

	float ao = 0.0f;
	float rad = g_sample_rad/z;
	
	//return float4(rad.xxx/500,1);
	
	//return doAmbientOcclusion(Pos.xy, Pos.xy, p, n);
	
//o_color = float4(rand.xxx,1);
	int iterations = lerp(6.0,2.0,z/ssao_iterations); 
	//int iterations = ssao_iterations;
	for (int j = 0; j < iterations; ++j)
	{
		  float2 coord1 = reflect(vec[j],rand)*rad;
		  float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707,
					  coord1.x*0.707 + coord1.y*0.707);
		  
		  ao += doAmbientOcclusion(Pos.xy,coord1*0.25, p, n, z);
		  ao += doAmbientOcclusion(Pos.xy,coord2*0.5, p, n, z);
		  ao += doAmbientOcclusion(Pos.xy,coord1*0.75, p, n, z);
		  ao += doAmbientOcclusion(Pos.xy,coord2, p, n, z);
	}
	ao/=(float)iterations*4.0;
	
	float4 o_color = float4(ao, ao, ao, ao);
	//o_color = float4(1,1,1,1);
	o_color.a = 1;
	//o_color = 1-o_color;
	return o_color;
}
