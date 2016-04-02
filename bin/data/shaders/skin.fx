#include "contants/ctes_camera.h"
#include "contants/ctes_object.h"
#include "contants/ctes_bones.h"

//--------------------------------------------------------------------------------------
void VSSkin(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL
	, in float2 iTex0 : TEXCOORD0
	, in int4   iBones : BONES
	, in float4 iWeights : WEIGHTS
	, out float4 oPos : SV_POSITION
	, out float4 oCol : COLOR
	, out float2 oTex0 : TEXCOORD0
	//  , out float3 oNormal : NORMAL
	)
{
	// This matrix will be reused for the position, Normal, Tangent, etc
	float4x4 skin_mtx = Bones[iBones.x] * iWeights.x
		+ Bones[iBones.y] * iWeights.y
		+ Bones[iBones.z] * iWeights.z
		+ Bones[iBones.w] * iWeights.w;

	float4 skinned_Pos = mul(iPos, skin_mtx);

	float4 worldPos = mul(skinned_Pos, World);

	worldPos.xyz *= 10.0f;

	float4 norm = normalize(mul(Rotation, iNormal));
	float diffusebrightness = saturate(dot(norm, lightvec));
	oCol = ambientcol;		//ambient as base color
	oCol += lightcol * diffusebrightness;

	oPos = mul(worldPos, ViewProjection);
	oTex0 = iTex0;
	//  oNormal = mul(iNormal, (float3x3)skin_mtx);
}