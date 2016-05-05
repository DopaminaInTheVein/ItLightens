#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_light.h"

Texture2D txDiffuse   : USE_SHADER_REG(TEXTURE_SLOT_DIFFUSE);
Texture2D txNormal    : USE_SHADER_REG(TEXTURE_SLOT_NORMALS);
Texture2D txWorldPos  : USE_SHADER_REG(TEXTURE_SLOT_WORLD_POS);
Texture2D txLightMask : USE_SHADER_REG(TEXTURE_SLOT_LIGHT_MASK);
Texture2D txSelfIlum : USE_SHADER_REG(TEXTURE_SLOT_SELFILUM);

TextureCube txEnvironment : USE_SHADER_REG(TEXTURE_SLOT_ENVIRONMENT);

// Same order as
SamplerState samLinear : register(s0);
SamplerState samLightBlackBorder : register(s1);
SamplerState samClampLinear : register(s2);

//--------------------------------------------------------------------------------------
void VS(
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
  oWorldPos = worldPos;
}

//--------------------------------------------------------------------------------------
// Only output the position in projection space
// Used ONLY by the point lights to compute lights
void VS_Pass(
  in float4 iPos : POSITION
  , out float4 oPos : SV_POSITION
  )
{
  oPos = iPos;
}

void VSLightDir(
  in float4 iPos : POSITION
  , out float4 oPos : SV_POSITION
  )
{
  float4 worldPos = mul(iPos, World);
  oPos = mul(worldPos, ViewProjection);
}

//--------------------------------------------------------------------------------------
void PSGBuffer(
  float4 Pos : SV_POSITION
  , float3 iNormal : NORMAL0
  , float4 iTangent : NORMAL1
  , float2 iTex0 : TEXCOORD0
  , float3 iWorldPos : TEXCOORD1
  , out float4 o_albedo : SV_Target0
  , out float4 o_normal : SV_Target1
  , out float4 o_wpos : SV_Target2
  , out float4 o_selfIlum : SV_Target3
  )
{
  o_albedo = txDiffuse.Sample(samLinear, iTex0);

  // Generar la matrix TBN usando la informacion interpolada
  // desde los 3 vertices del triangulo
  float3 T = normalize(iTangent.xyz);
  float3 N = normalize(iNormal);
  float3 B = cross(N, T) * iTangent.w;
  float3x3 TBN = float3x3(T, -B, N);

  // Leer la normal en tangent space tal como esta en la textura
  // y convertir el rango de 0..1 a -1..1
  float3 N_tangent_space = txNormal.Sample(samLinear, iTex0).xyz * 2. - 1.;
  // Cambiar la intensidad del normal map
  //N_tangent_space.xy *= 0.5;
  //N_tangent_space = normalize(N_tangent_space);
  float3 N_world_space = mul(N_tangent_space, TBN);

  o_normal.xyz = (normalize(N_world_space) + 1.) * 0.5;
  o_normal.a = 1.;
  o_wpos = float4(iWorldPos, 1.);

  o_selfIlum = txSelfIlum.Sample(samLinear, iTex0);
  if (!(o_selfIlum.r == 0 && o_selfIlum.g == 0 && o_selfIlum.b == 0))
	  o_selfIlum *= float4(1, 0, 0, 1);
}

//--------------------------------------------------------------------------------------
void PSLightPoint(
  in float4 iPosition : SV_Position
  , out float4 o_color : SV_Target
  )
{
  int3 ss_load_coords = uint3(iPosition.xy, 0);

  // Recuperar la posicion de mundo para ese pixel
  float3 wPos = txWorldPos.Load(ss_load_coords).xyz;

  // Recuperar la normal en ese pixel. Sabiendo que se
  // guardó en el rango 0..1 pero las normales se mueven
  // en el rango -1..1
  float3 N = txNormal.Load(ss_load_coords).xyz * 2 - 1;

  // Vector L desde la world pos to the light center
  float3 L = LightWorldPos.xyz - wPos;
  float  d2L = length(L);
  L = L / d2L;

  // Calculo luz diffuso basico
  // Saturate limita los valores de salida al rango 0..1
  float NL = saturate(dot(N, L));

  // Factor de atenuacion por distancia al centro de la
  // luz. 1 para distancias menores de LightInRadius y 0
  // para distancias mayores que LightOutRadius. Entre medio
  // interpolacion lineal. Cambiarlo a lo que gusteis.
  float distance_att = 1. - saturate((d2L - LightInRadius) / (LightOutRadius - LightInRadius));

  float4 albedo = txDiffuse.Load(ss_load_coords);

  // Calculo el vector E normalizado
  float3 E = normalize(CameraWorldPos.xyz - wPos);

  float3 H = normalize(E + L);
  float  cos_beta = saturate(dot(N, H));
  float  glossiness = 20.;
  float  spec_amount = pow(cos_beta, glossiness);
  spec_amount *= distance_att;

  // Environment. incident_vector = -E
  float3 E_refl = reflect(-E, N);
  float3 env = txEnvironment.Sample(samLinear, E_refl).xyz;

  // Aportacion final de la luz es NL x color_luz x atenuacion
  o_color.xyz = LightColor.xyz * NL * distance_att * albedo + spec_amount;
  o_color.xyz += env * 0.3;
  //o_color.xyz = E_refl.xyz;
  o_color.a = 1.;

  //o_color = float4(NL, NL, NL ,1) * albedo;
}

//--------------------------------------------------------------------------------------
float4 PSLightDir(
  in float4 iPosition : SV_Position
  ) : SV_Target
{
  int3 ss_load_coords = uint3(iPosition.xy, 0);

  // Recuperar la posicion de mundo para ese pixel
  float3 wPos = txWorldPos.Load(ss_load_coords).xyz;

  // Recuperar la normal en ese pixel. Sabiendo que se
  // guardó en el rango 0..1 pero las normales se mueven
  // en el rango -1..1
  float3 N = txNormal.Load(ss_load_coords).xyz * 2 - 1;

  // Vector L desde la world pos to the light center
  float3 L = LightWorldPos.xyz - wPos;
  float  d2L = length(L);
  L = L / d2L;

  // Calculo luz diffuso basico
  // Saturate limita los valores de salida al rango 0..1
  float NL = saturate(dot(N, L));

  // La luz direccional tiene un alcance maximo
  float distance_att = 1. - saturate(d2L / LightOutRadius);

  // Attenuacion por direccion
  //float cos_angle = saturate( dot( LightWorldFront, -L ) );
  //float ratio = (cos_angle - LightCosFov ) / ( 1. - LightCosFov );
  //distance_att *= 3 * ratio;

  // Comprobar si estoy en el cono de vision de la luz direccional
  // Pasar a espacio homogeneo de la luz
  float4 lightSpacePos = mul(float4(wPos,1), LightViewProjection);
  lightSpacePos.xyz /= lightSpacePos.w;
  lightSpacePos.x = (lightSpacePos.x + 1) * 0.5;
  lightSpacePos.y = (1 - lightSpacePos.y) * 0.5;

  float4 light_mask = txLightMask.Sample(samLightBlackBorder, lightSpacePos.xy );
  if( lightSpacePos.w < 0 )
	light_mask.xyzw = .0;

  float4 final_color = light_mask * NL;

  float4 albedo = txDiffuse.Load(ss_load_coords);

  return final_color * albedo;
  
  /*
 

  // Calculo el vector E normalizado
  float3 E = normalize(CameraWorldPos.xyz - wPos);

  float3 H = normalize(E + L);
  float  cos_beta = saturate(dot(N, H));
  float  glossiness = 20.;
  float  spec_amount = pow(cos_beta, glossiness);
  spec_amount *= distance_att;

  // Environment. incident_vector = -E
  float3 E_refl = reflect(-E, N);
  float3 env = txEnvironment.Sample(samLinear, E_refl).xyz;

  // Aportacion final de la luz es NL x color_luz x atenuacion
  o_color.xyz = LightColor.xyz * NL * distance_att * albedo + spec_amount;
  o_color.xyz += env * 0.3;
  //o_color.xyz = E_refl.xyz;
  o_color.a = 1.;

  //o_color = float4(NL, NL, NL ,1) * albedo;
*/
}



//--------------------------------------------------------------------------------------
float4 PSWater(
  in float4 iPosition : SV_Position
  , in float3 iNormal : NORMAL0
  , in float4 iTangent : NORMAL1
  , in float2 iTex0 : TEXCOORD0
  , in float3 iWorldPos : TEXCOORD1
  ) : SV_Target
{
  int3 ss_load_coords = uint3(iPosition.xy, 0);

  // Recuperar la posicion de mundo para ese pixel
  float4 under_water = txDiffuse.Load(ss_load_coords + uint3(20,0,0));

  // fresnel-like effect
  float3 E = normalize(CameraWorldPos.xyz - iWorldPos);
  float VdotN = dot( iNormal, E );
  float f = saturate(1-VdotN);
  f = pow( f, 2 );
  float4 color2 = float4(1,0,0,1);
  
  return under_water * (1-f) + color2 * (f);
}

//--------------------------------------------------------------------------------------
void VS_NO_TANGENT(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL0
	, in float2 iTex0 : TEXCOORD0
	, out float4 oPos : SV_POSITION
	, out float3 oNormal : NORMAL0
	, out float2 oTex0 : TEXCOORD0
	, out float3 oWorldPos : TEXCOORD1
	)
{
	float4 worldPos = mul(iPos, World);
	oPos = mul(worldPos, ViewProjection);

	// Rotar la normal segun la transform del objeto
	oNormal = mul(iNormal, (float3x3)World);

	// Las uv's se pasan directamente al ps
	oTex0 = iTex0;
	oWorldPos = worldPos;
}


//--------------------------------------------------------------------------------------
void PSGBuffer_NO_TANGENT(
	float4 Pos : SV_POSITION
	, float3 iNormal : NORMAL0
	, float2 iTex0 : TEXCOORD0
	, float3 iWorldPos : TEXCOORD1
	, out float4 o_albedo : SV_Target0
	, out float4 o_normal : SV_Target1
	, out float4 o_wpos : SV_Target2
	)
{
	o_albedo = txDiffuse.Sample(samLinear, iTex0);

	float3 N = normalize(iNormal);


	o_normal.xyz = (N + 1.) * 0.5;
	o_normal.a = 1.;
	o_wpos = float4(iWorldPos, 1.);
}


//TEMPORAL OBJECTS NO TANGENT NO TEX


//--------------------------------------------------------------------------------------
void VS_NO_TANGENT_NO_TEX(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL0
	, in float2 iTex0 : TEXCOORD0
	, out float4 oPos : SV_POSITION
	, out float3 oNormal : NORMAL0
	, out float2 oTex0 : TEXCOORD0
	, out float3 oWorldPos : TEXCOORD1
	)
{
	float4 worldPos = mul(iPos, World);
	oPos = mul(worldPos, ViewProjection);

	// Rotar la normal segun la transform del objeto
	oNormal = mul(iNormal, (float3x3)World);

	// Las uv's se pasan directamente al ps
	oTex0 = iTex0;
	oWorldPos = worldPos;
}


//--------------------------------------------------------------------------------------
void PSGBuffer_NO_TANGENT_NO_TEX(
	float4 Pos : SV_POSITION
	, float3 iNormal : NORMAL0
	, float2 iTex0 : TEXCOORD0
	, float3 iWorldPos : TEXCOORD1
	, out float4 o_albedo : SV_Target0
	, out float4 o_normal : SV_Target1
	, out float4 o_wpos : SV_Target2
	)
{
	o_albedo = txDiffuse.Sample(samLinear, iTex0);

	float3 N = normalize(iNormal);


	o_normal.xyz = (N + 1.) * 0.5;
	o_normal.a = 1.;
	o_wpos = float4(iWorldPos, 1.);
}