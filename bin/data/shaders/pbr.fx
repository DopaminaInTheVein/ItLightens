#include "globals.fx"

#define PI 3.14159265359f

//--------------------------------------------------------------------------------------
// Same as in deferred
void VS_GBuffer(
  in float4 iPos : POSITION
, in float3 iNormal : NORMAL0
, in float2 iTex0 : TEXCOORD0
, in float4 iTangent : NORMAL1
, out float4 oPos : SV_POSITION
, out float3 oNormal : NORMAL0
, out float4 oTangent : NORMAL1
, out float2 oTex0 : TEXCOORD0
, out float3 oWorldPos : TEXCOORD1
)
{
  float4 worldPos = mul(iPos, World);
  oPos = mul(worldPos, ViewProjection);

  // Rotar la normal segun la transform del objeto
  oNormal = mul(iNormal, (float3x3)World);
  oTangent.xyz = mul(iTangent.xyz, (float3x3)World);
  oTangent.w = iTangent.w;

  // Las uv's se pasan directamente al ps
  oTex0 = iTex0;
  oWorldPos = worldPos.xyz;
}

//--------------------------------------------------------------------------------------
void decodeGBuffer( 
     in float2 iPosition
   , out float3 wPos 
   , out float3 N 
   , out float3 real_albedo 
   , out float3 real_specular_color
   , out float  roughness 
   , out float3 reflected_dir
   , out float3 view_dir
   ) {

  int3 ss_load_coords = uint3(iPosition.xy, 0);

  // Recuperar la posicion de mundo para ese pixel
  float  z = txDepths.Load(ss_load_coords).x;
  wPos = getWorldCoords(iPosition.xy, z);

  // Recuperar la normal en ese pixel. Sabiendo que se
  // guardó en el rango 0..1 pero las normales se mueven
  // en el rango -1..1
  float4 N_rt = txNormal.Load(ss_load_coords);
  N = N_rt.xyz * 2 - 1;

  // Get other inputs from the GBuffer
  float4 albedo = txDiffuse.Load(ss_load_coords);
  float  metallic = albedo.a;
         roughness = N_rt.a;
 
  // Apply gamma correction to albedo to bring it back to linear.
  albedo.rgb = pow(albedo.rgb, 2.2f);

  // Lerp with metallic value to find the good diffuse and specular.
  // If metallic = 0, albedo is the albedo, if metallic = 1, the
  // used albedo is almost black
  real_albedo = albedo.rgb * ( 1. - metallic );

  // 0.03 default specular value for dielectric.
  real_specular_color = lerp(0.03f, albedo.rgb, metallic);

  // Eye to object
  float3 incident_dir = normalize(wPos - CameraWorldPos.xyz);
  reflected_dir = normalize(reflect(incident_dir, N));
  view_dir = -incident_dir;
}

// -------------------------------------------------
// Gloss = 1 - rough*rough
float3 Specular_F_Roughness(float3 specularColor, float gloss, float3 h, float3 v) {
  // Sclick using roughness to attenuate fresnel.
  return (specularColor + (max(gloss, specularColor) - specularColor) * pow((1 - saturate(dot(v, h))), 5));
}

float NormalDistribution_GGX(float a, float NdH)
{
    // Isotropic ggx.
    float a2 = a*a;
    float NdH2 = NdH * NdH;

    float denominator = NdH2 * (a2 - 1.0f) + 1.0f;
    denominator *= denominator;
    denominator *= PI;

    return a2 / denominator;
}

float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
        // Smith schlick-GGX.
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}


float Specular_D(float a, float NdH)
{
    return NormalDistribution_GGX(a, NdH);
}

float Specular_G(float a, float NdV, float NdL, float NdH, float VdH, float LdV) 
{
    return Geometric_Smith_Schlick_GGX( a, NdV, NdL );
}

float3 Fresnel_Schlick(float3 specularColor, float3 h, float3 v)
{
    return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}

float3 Specular_F(float3 specularColor, float3 h, float3 v)
{
    return Fresnel_Schlick(specularColor, h, v);
}

float3 Specular(float3 specularColor, float3 h, float3 v, float3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV)
{
    return ((Specular_D(a, NdH) * Specular_G(a, NdV, NdL, NdH, VdH, LdV)) * Specular_F(specularColor, v, h) ) / (4.0f * NdL * NdV + 0.0001f);
}

//--------------------------------------------------------------------------------------
void PS_GBuffer(
  float4 Pos : SV_POSITION
, float3 iNormal : NORMAL0
, float4 iTangent : NORMAL1
, float2 iTex0 : TEXCOORD0
, float3 iWorldPos : TEXCOORD1
, out float4 o_albedo : SV_Target0
, out float4 o_normal : SV_Target1
, out float1 o_depth : SV_Target2
)
{
  o_albedo = txDiffuse.Sample(samLinear, iTex0);
  o_albedo.a = txMetallic.Sample(samLinear, iTex0).r;

  // Generar la matrix TBN usando la informacion interpolada
  // desde los 3 vertices del triangulo
  float3 T = normalize(iTangent.xyz);
  float3 N = normalize(iNormal);
  float3 B = cross(N, T) * iTangent.w;

  // ojo al cambio de signos, el N que he cogido no sigue el patron 
  // que usamos en clase. 
  float3x3 TBN = float3x3(-T, -B, N);   

  // Leer la normal en tangent space tal como esta en la textura
  // y convertir el rango de 0..1 a -1..1
  float3 N_tangent_space = txNormal.Sample(samLinear, iTex0).xyz * 2. - 1.;
  // Cambiar la intensidad del normal map
  //N_tangent_space.xy *= 0.5;
  //N_tangent_space = normalize(N_tangent_space);
  float3 N_world_space = mul(N_tangent_space, TBN);

  o_normal.xyz = (normalize(N_world_space) + 1.) * 0.5;

  // Save roughness in the alpha coord of the N render target
  o_normal.a = txRoughness.Sample(samLinear, iTex0).r;

  // Si el material lo pide, sobreescribir los valores de la textura
  // por unos escalares uniformes
  if (scalar_metallic >= 0.f)
    o_albedo.a = scalar_metallic;
  if (scalar_roughness >= 0.f)
    o_normal.a = scalar_roughness;

  float3 camera2wpos = iWorldPos - CameraWorldPos.xyz;
  o_depth = dot( CameraFront.xyz, camera2wpos) / CameraZFar;
}




//--------------------------------------------------------------------------------------
//-
//--------------------------------------------------------------------------------------
void VS_fullquad(
    in float4 iPos : POSITION
  , in float2 iTex0 : TEXCOORD0
  , out float4 oPos : SV_POSITION
  )
{
  // Passthrough of coords and UV's
  oPos = float4(iPos.xyz, 1);
}

//--------------------------------------------------------------------------------------
float4 PS_ambient(
  in float4 iPosition : SV_Position
) : SV_Target
{

  float3 wPos, N, albedo, specular_color, reflected_dir, view_dir;
  float  roughness;
  decodeGBuffer( iPosition.xy, wPos, N, albedo, specular_color, roughness, reflected_dir, view_dir );

  float mipIndex = roughness * roughness * 8.0f;
  float3 env = txEnvironment.SampleLevel(samLinear, reflected_dir, mipIndex).xyz;
  env = pow(env, 2.2f);

  float3 irradiance_mipmaps = txEnvironment.SampleLevel(samLinear, N, 4).xyz;
  float3 irradiance_texture = txIrradiance.Sample(samLinear, N).xyz;
  float3 irradiance = irradiance_texture * scalar_irradiance_vs_mipmaps + irradiance_mipmaps * ( 1. - scalar_irradiance_vs_mipmaps );

  float3 env_fresnel = Specular_F_Roughness(specular_color, 1. - roughness * roughness, N, view_dir);
  //return float4(env_fresnel, 1 );

  float g_ReflectionIntensity = 1.0;
  float g_AmbientLightIntensity = 1.0;

  float4 final_color = float4(env_fresnel * env * g_ReflectionIntensity + 
                              albedo.xyz * irradiance * g_AmbientLightIntensity
                              , 1.0f);

  //return float4(0,0,0,0);

  return final_color;
  //return pow( final_color, 1./2.2);

  // El monitor introduce un pow( 2.2 ) a la señal que nosotros 
  // le enviemos...
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void VS_pass(
  in float4 iPos : POSITION
, in float3 iNormal : NORMAL0
, in float2 iTex0 : TEXCOORD0
, out float4 oPos : SV_POSITION
) {
  float4 worldPos = mul(iPos, World);
  oPos = mul(worldPos, ViewProjection);
}

float3 Diffuse(float3 pAlbedo) {
    return pAlbedo/PI;
}

float4 PS_point_lights(
  in float4 iPosition : SV_Position
) : SV_Target
{
  float3 wPos, N, albedo, specular_color, reflected_dir, view_dir;
  float  roughness;

  decodeGBuffer( iPosition.xy, wPos, N, albedo, specular_color, roughness, reflected_dir, view_dir );

  // From wPos to Light
  float3 light_dir = normalize(LightWorldPos.xyz - wPos);

  float  NdL = saturate(dot(N, light_dir));
  float  NdV = saturate(dot(N, view_dir));
  float3 h   = normalize(light_dir + view_dir); // half vector

  float  NdH = saturate(dot(N, h));
  float  VdH = saturate(dot(view_dir, h));
  float  LdV = saturate(dot(light_dir, view_dir));
  float  a   = max(0.001f, roughness * roughness);
  float3 cDiff = Diffuse(albedo);
  float3 cSpec = Specular(specular_color, h, view_dir, light_dir, a, NdL, NdV, NdH, VdH, LdV);

  float3 final_color = LightColor * NdL * (cDiff * (1.0f - cSpec) + cSpec);
  return float4( final_color, 1 );
}