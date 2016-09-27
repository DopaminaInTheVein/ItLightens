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

//--------------------------------------------------------------------------------------
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
} 
