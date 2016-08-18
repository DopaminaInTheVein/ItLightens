#include "globals.fx"

float Heaviside(float R, float3 w)
{
   float W = length(w);
 
   float test = R - W;
 
   if(test < 0)
     return 0.0f;
   else 
     return 1.0f;
}


float MonteCarloApproximation(float3 N, float3 w, float c, float R, float depth)
{
   float Approx = 0.0f;
 
   float numerator = max(0, dot(N, w) - 0.001 * depth) * Heaviside(R, w);
 
   float denominator = max((c * c), dot(w, w));
 
   Approx = numerator / denominator;
 
   return Approx;
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
float4 PS(float4 Pos : SV_POSITION
	, float2 iTex0 : TEXCOORD0
	) : SV_Target
{
	//Sample data from buffers
   float2 vTexCoord = iTex0;
   
   int3 ss_load_coords = uint3(Pos.xy, 0);
   
  // Recuperar la posicion de mundo para ese pixel
  float  z = txDepths.Load(ss_load_coords).x;
  //float z_scaled = z*2.0f;
  float3 position = getWorldCoords(Pos.xy, z);
  
   float3 normal = txNormal.Load(ss_load_coords).xyz * 2 - 1;
 
   //Calculate the depth in view space (values needed to be scaled down)
   float4 vDepth = (mul(float4(position, 1), ViewProjection) * -1) / 5 ;
	
   //z /= 5;
   
   //Range of influence
   float R = 3.50f;
 
   //The point at which the fall-off function reaches its peak of 1
   float c = 0.1f * R;
 
   //Number of samples used
   int n = 40;
 //z-=0.1;
   //Calculate the sampling distance and step size
   float sArea = R / (z/5.0f);
   float sStep = sArea / n;
 
   //Starting angle is read in from a noise texture for a randomization factor
   float4 random = txNoise.Sample(samLinear, iTex0);
   float angle = random.r+random.g;
   angle /= (random.b);
   //angle = 1;
   angle *= 2.0f * PI;
 
   //Calculates how many steps will need to be taken to go full circle
   float angleStep = 2.0f * PI / n;
 
   //Initialize the Monte-Carlo sum value
   float monteCarlo = 0.0f;
 
   //Iterate through all samples
   for(int i = 0; i < n; ++i)
   {
      //Change the sampling coordinates for the next sample
      float2 pixelOffset = float2(sStep * cos(angle), sStep * sin(angle));
	  float2 pixel_sample = Pos + pixelOffset;
 
	int3 ss_load_coords_sample = uint3(pixel_sample.xy, 0);
   
  // Recuperar la posicion de mundo para ese pixel
  float  z_sample = txDepths.Load(ss_load_coords_sample).x;
  //z_sample /= 5.0;
  
    //Calculate the depth in view space (values needed to be scaled down)
   
 
  
  float3 pixelPosition_sample = getWorldCoords(pixel_sample.xy, z_sample);
  
 
      //Vector from the current position to the position of the sampled pixel
      float3 w = pixelPosition_sample - position;
 
      //Adds th next step in the Monte-Carlo integral
      monteCarlo += MonteCarloApproximation(normal, w, c, R, z);
 
      //Offsets the angle for the next sampling
      angle += angleStep;
   }
 
   //A factor that will multiply the final Monte-Carlo sum
   float factor = ((2.0f * PI * c) / n);
 
   //Calculating the final occlusion value
   float A = 1-(factor * monteCarlo);
 
   //A = A*A*A;
 
   return float4(A, A, A, 1)/2.0f;
}
