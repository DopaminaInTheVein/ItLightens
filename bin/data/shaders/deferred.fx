#include "globals.fx"

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

//--------------------------------------------------------------------------------------
// When we are only interested in rasterizing the geometry
// we use this vertex shader. No coords, tangents, normals...
void VSOnlyPos(
  in float4 iPos : POSITION
  , out float4 oPos : SV_POSITION
  )
{
  float4 worldPos = mul(iPos, World);
  oPos = mul(worldPos, ViewProjection);
}

//--------------------------------------------------------------------------------------
void VSOnlyPosSkin(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL
	, in float2 iTex0 : TEXCOORD0
	, in float4 iTangent : NORMAL1
	, in int4   iBones : BONES
	, in float4 iWeights : WEIGHTS
	, out float4 oPos : SV_POSITION
	)
{
	// This matrix will be reused for the position, Normal, Tangent, etc
	float4x4 skin_mtx = Bones[iBones.x] * iWeights.x
		+ Bones[iBones.y] * iWeights.y
		+ Bones[iBones.z] * iWeights.z
		+ Bones[iBones.w] * iWeights.w;

	//Position
	float4 skinned_Pos = mul(iPos, skin_mtx);
	oPos = mul(skinned_Pos, ViewProjection);
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
  , out float1 o_depth : SV_Target2
  , out float4 o_selfIlum : SV_Target3
  , out float4 o_speculars : SV_Target4
  , out float4 o_glossiness : SV_Target5
  )
{
  o_albedo = txDiffuse.Sample(samLinear, iTex0);
  o_speculars = txSpecular.Sample(samLinear, iTex0);
  if(o_speculars.a < 1.0f)
	o_glossiness = float4(o_speculars.a, o_speculars.a, o_speculars.a, 1);
	
  else
	o_glossiness = float4(0.f, 0.f, 0.f, 1);
	
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
  //o_specular.a =
  float3 camera2wpos = iWorldPos - CameraWorldPos.xyz;
  o_depth = dot( CameraFront.xyz, camera2wpos) / CameraZFar;

  o_selfIlum = txSelfIlum.Sample(samLinear, iTex0);
  float limit = 0.15f;
  
  if (length(o_selfIlum.xyz) > limit)
	  o_albedo = o_selfIlum;
}

//--------------------------------------------------------------------------------------
void PSTransparency(
  float4 Pos : SV_POSITION
  , float3 iNormal : NORMAL0
  , float4 iTangent : NORMAL1
  , float2 iTex0 : TEXCOORD0
  , float3 iWorldPos : TEXCOORD1
  , out float4 o_color : SV_Target0
  , out float4 o_normal : SV_Target1
  , out float4 o_depth : SV_Target2
   , out float4 o_glossiness  : SV_Target3
  , out float4 o_speculars : SV_Target4
  )
{
  float4 o_albedo = txDiffuse.Sample(samLinear, iTex0);
  //float4 o_speculars = txSpecular.Sample(samLinear, iTex0);
  //float4 o_glossiness = float4(o_speculars.a, o_speculars.a, o_speculars.a, 1);
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
  //float4 o_normal;
  o_normal.xyz = (normalize(N_world_space) + 1.) * 0.5;
  
  float3 camera2wpos = iWorldPos - CameraWorldPos.xyz;
  o_depth = dot( CameraFront.xyz, camera2wpos) / CameraZFar;
  
  //o_normal.a = o_albedo.a;
  o_color = o_albedo;
  o_color.a = o_albedo.a*4;
  //o_color = float4(1,1,1,1);
  
  o_normal.a = o_color.a;
  
  //depth for transparencies need a limit, can't merge different depths
  if(o_color.a > 0.1)
	o_depth.a = 1;
 else 
	o_depth.a = 0;
  //o_color.a = 1;
  
  o_glossiness = float4(0,0,0,0);
  o_glossiness.a = o_color.a;
  o_speculars = o_glossiness;
  
}

//--------------------------------------------------------------------------------------
void PSLightPoint(
  in float4 iPosition : SV_Position
  , out float4 o_color : SV_Target0
  , out float4 o_specular : SV_Target1
  , out float4 o_inv_shadows : SV_Target2
  )
{
  int3 ss_load_coords = uint3(iPosition.xy, 0);

  // Recuperar la posicion de mundo para ese pixel
  float  z = txDepths.Load(ss_load_coords).x;
  float3 wPos = getWorldCoords(iPosition.xy, z);

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
  
 float4 lightWarp = txWarpLight.Sample(samClampLinear, float2(NL, 0.0f))*2.0f;
  if(use_ramp)
	NL = lightWarp.xyz;

  if(NL > 1)
	NL = 1;
  // Factor de atenuacion por distancia al centro de la
  // luz. 1 para distancias menores de LightInRadius y 0
  // para distancias mayores que LightOutRadius. Entre medio
  // interpolacion lineal. Cambiarlo a lo que gusteis.
  float distance_att = 1. - saturate((d2L - LightInRadius) / (LightOutRadius - LightInRadius));

  float4 albedo = txDiffuse.Load(ss_load_coords);
  float4 specular_color = txSpeculars.Load(ss_load_coords); 
  float  glossiness = txGlossiness.Load(ss_load_coords).r;
  
  float3 E = normalize(CameraWorldPos.xyz - wPos.xyz);


  float3 H = normalize(E + L);
  float  cos_beta = saturate(dot(N, H));
  //glossiness *= 10;
  glossiness *= 100.0f;

  //float spec_reflec = pow(cos_beta, 20);
  float spec_amount = pow(cos_beta, (255-glossiness*3)/10);
 
  //spec_amount += spec_reflec*glossiness;
  spec_amount *= distance_att;
  
  //spec_amount /= 2.0f;

  // Environment. incident_vector = -E
  //float3 E_refl = reflect(-E_fixed, N/2);
  
  
  
  
	//if(NL < 1.0f)
		//NL = 1.0f;

  //noramlize light with intensity
  float3 lightCol = LightColor.xyz*LightColor.a;
  
  
  if(color_ramp != 0.0f){
	//lightCol *= lightWarp.xyz;
  }
  
  //spec_amount *= specular_force;
  o_specular = spec_amount*specular_force;
  o_specular.a = 1;
  //o_specular += spec_reflec*specular_color/5.0f;
  //spec_amount *= 0;
  
  //if(length(specularForce) > 0.1f)
	//o_specular = float4(1,1,1,1);
  
  float inv_shadows = NL*distance_att;
  if(inv_shadows > 1)
	inv_shadows = 1;
	
 // inv_shadows = 1 - inv_shadows;
  //inv_shadows = 1 - saturate(inv_shadows);
 // inv_shadows /= 2;
  o_inv_shadows = float4(inv_shadows, inv_shadows, inv_shadows, 1);
  
  // Aportacion final de la luz es NL x color_luz x atenuacion
  o_color.xyz = lightCol * NL * distance_att * albedo + o_specular.xyz*specular_color;
 
  //o_color.xyz += env * glossiness * 0.02;
  //o_color.xyz = env.xyz;
  //o_color.xyz = E_refl.xyz;
  //o_color = length((CameraWorldPos - pos_env));
  o_color.a = 1.;
  
  //o_inv_shadows = float4(1,1,1,1);
  o_inv_shadows = float4(0,0,0,0);
  
  //o_color = float4(spec_amount, spec_amount, spec_amount, 1.0f);
  
  //o_color = float4(wPos, 1);
  
  

  //o_color = float4(NL, NL, NL ,1) * albedo;
}

//--------------------------------------------------------------------------------------
void PSLightDir(
  in float4 iPosition : SV_Position
  , out float4 o_color : SV_Target0
  , out float4 o_specular : SV_Target1)
{
  int3 ss_load_coords = uint3(iPosition.xy, 0);

  // Recuperar la posicion de mundo para ese pixel
  float  z = txDepths.Load(ss_load_coords).x;
  float3 wPos = getWorldCoords(iPosition.xy, z);

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
  
  //rim specular
  //rim specular_force
  float3 eye = - (CameraWorldPos.xyz - wPos);
  eye = normalize(eye);
  float rim = 2.0f - NL;
  rim = smoothstep(0.6, 1.0, rim);
  
  float rim_force = rim_specular;
  if(rim_force > 0)
	rim *= max(N.y - length(N.xz)/rim_force, 0);	//rim only vector up
  else
	rim = 0;
	
  o_specular = float4(rim,rim,rim,0.f) * distance_att;
  o_specular.a = 1.0f;

  float4 albedo = txDiffuse.Load(ss_load_coords);

  o_color = final_color * albedo;
  
}


//--------------------------------------------------------------------------------------
float tapAt(float2 homo_coords, float depth) {
  float amount = txShadowMap.SampleCmpLevelZero(samPCFShadows
    , homo_coords, depth);
	
	float amount_static = txShadowMapStatic.SampleCmpLevelZero(samPCFShadows
	, homo_coords, depth);
	
	if(amount < amount_static)
		return amount; 
	else
		return amount_static;
}

// -------------------------
float getShadowAt(float4 wPos) {

  // Move to homogeneous space of the light
  float4 light_proj_coords = mul(wPos, LightViewProjectionOffset);
  light_proj_coords.xyz /= light_proj_coords.w;
  
  if (light_proj_coords.z < 1e-3)
    return 0.;

	
   float2 rand = txNoise.Sample(samLinear, float2(wPos.xy)).xy ;/// light_proj_coords.z ;
  
  float2 center = light_proj_coords.xy;
  float depth = light_proj_coords.z - 0.001;	
  float amount = tapAt(center, depth);
  
 
  float2 sz; //= float2(2.0/ xres, 2.0/yres)*light_proj_coords.z;

  float step_sz = 2.0f;
  sz = float2(step_sz/xres, step_sz/yres);
  float steps = 1.0f;
  float2 pos = center;
  for(int i=1; i<= steps; i++){
	 pos = center + txNoise.Sample(samLinear, pos).xy * sz ;
  
	  amount += tapAt(pos + float2(sz.x, sz.y)*i, depth);
	  amount += tapAt(pos + float2(-sz.x, sz.y)*i, depth);
	  amount += tapAt(pos + float2(sz.x, -sz.y)*i, depth);
	  amount += tapAt(pos + float2(-sz.x, -sz.y)*i, depth);

	  amount += tapAt(pos + float2(sz.x, 0)*i, depth);
	  amount += tapAt(pos + float2(-sz.x, 0)*i, depth);
	  amount += tapAt(pos + float2(0, -sz.y)*i, depth);
	  amount += tapAt(pos + float2(0, sz.y)*i, depth);
 }
  
  amount *= 1.f / (8. * steps + 1);
  
  
  return amount;
}


//--------------------------------------------------------------------------------------
void PSLightDirShadows(
  in float4 iPosition : SV_Position
  , out float4 o_color  : SV_Target0
  , out float4 o_specular : SV_Target1
  , out float4 o_inv_shadows : SV_Target2
  ) 
{
	int3 ss_load_coords = uint3(iPosition.xy, 0);

	// Recuperar la posicion de mundo para ese pixel
	float  z = txDepths.Load(ss_load_coords).x;
	float3 wPos = getWorldCoords(iPosition.xy, z);

	// Recuperar la normal en ese pixel. Sabiendo que se
	// guardó en el rango 0..1 pero las normales se mueven
	// en el rango -1..1
	float3 N = txNormal.Load(ss_load_coords).xyz * 2 - 1;

	// Vector L desde la world pos to the light center
	float3 L = LightWorldPos.xyz - wPos;
	float  d2L = length(L);
	L = L / d2L;
	//L = float3(1,1,1);
	//N = float3(1,1,1);

	// Calculo luz diffuso basico
	// Saturate limita los valores de salida al rango 0..1
	float NL = saturate(dot(N, L));

	float4 lightWarp = txWarpLight.Sample(samClampLinear, float2(NL, 1.0f))*2.0f;
	float NLWarped = lightWarp.xyz;
	NLWarped = pow((NLWarped*0.5f + 0.5f),2);
	//NLWarped = NL;
  
	float4 lightSpacePos = mul(float4(wPos,1), LightViewProjection);
	lightSpacePos.xyz /= lightSpacePos.w;
	lightSpacePos.x = (lightSpacePos.x + 1) * 0.5;
	lightSpacePos.y = (1 - lightSpacePos.y) * 0.5;

	float4 light_mask = txLightMask.Sample(samLightBlackBorder, lightSpacePos.xy )-0.2;
	if( lightSpacePos.w < 0 )
		light_mask.xyzw = .0;
  
  // Currently, no attenuation based on distance
  // Attenuation based on shadowmap
	float att_factor = getShadowAt(float4(wPos, 1));
	float4 final_color = light_mask * att_factor * LightColor;
 
	if(generate_shadows){
		 float inv_shadows = att_factor;
		 o_inv_shadows = float4(inv_shadows, inv_shadows, inv_shadows, inv_shadows);
		  
		  

		  inv_shadows = 1-att_factor;
		 // inv_shadows = 1-att_factor*NLWarped;
		  inv_shadows *= (light_mask);
		  //inv_shadows *= (light_mask*light_mask);
		  
		  
		  //we reduce the shadow intesity, as the hatching will add more shadows effect
		  if(inv_shadows > shadow_intensity)
			inv_shadows = shadow_intensity;
			
		 if(inv_shadows < 0)
			inv_shadows = 0;
			
			//inv_shadows = 0;
		  o_inv_shadows = float4(inv_shadows, inv_shadows, inv_shadows, inv_shadows);
		  //o_color = float4(att_factor, att_factor, att_factor, att_factor);

  
	}else{
		o_inv_shadows = float4(0,0,0,0);
	}
	
	float4 specular_color = txSpeculars.Load(ss_load_coords); 
	float  glossiness = txGlossiness.Load(ss_load_coords).r;
  
	float3 E = normalize(CameraWorldPos.xyz - wPos.xyz);
	float3 H = normalize(E + L);
	float  cos_beta = saturate(dot(N, H));
	//glossiness *= 10;
	glossiness *= 100.0f;

	//float spec_reflec = pow(cos_beta, 20);
	float spec_amount = pow(cos_beta, (255-glossiness*3)/10);

	//spec_amount += spec_reflec*glossiness;
	spec_amount *= att_factor;
	
	
	o_specular = spec_amount*specular_force*light_mask*0.2f;
	//o_specular.a = 0;
	
	if(LightColor.a <= 0){
		o_specular = float4(0,0,0,0);
	}
	
	float4 albedo = txDiffuse.Load(ss_load_coords);
	o_color = (NLWarped * final_color * albedo + o_specular);
  
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
  float  z = txDepths.Load(ss_load_coords).x;
  float3 wPos = getWorldCoords(iPosition.xy, z);

  // Noise values between -1..1
  float2 delta = 10*iTex0;
  float4 noise = txNoise.Sample(samLinear, delta + world_time.xx * 0.2 ) * 2. - 1.;
  float4 noise2 = txNoise.Sample(samLinear, -iTex0 * 2.34 ) * 2. - 1.;
  
  wPos.x += noise.x * cos( world_time );
  wPos.z += noise.z * sin( world_time + 0.12);
  wPos.x += noise2.x * cos( world_time * 0.23 );
  wPos.z += noise2.z * sin( world_time * 1.8 + 0.12);
  
  // Pasar a espacio homogeneo de la camara
  float4 coords_vp = mul(float4(wPos,1), ViewProjection);
  coords_vp.xyz /= coords_vp.w;  
  coords_vp.x = (coords_vp.x + 1) * 0.5;
  coords_vp.y = (1 - coords_vp.y) * 0.5;

  // Recuperar la posicion de mundo para ese pixel
  //float4 under_water = txDiffuse.Load(ss_load_coords);
  float4 under_water = txDiffuse.Sample(samLinear, coords_vp );

  // fresnel-like effect
  float3 E = normalize(CameraWorldPos.xyz - iWorldPos);
  float VdotN = dot( iNormal, E );
  float f = saturate(1-VdotN);
  f = pow( f, 2 );
  float4 color2 = float4(0,0,0,1);
  
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