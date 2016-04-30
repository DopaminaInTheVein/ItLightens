#include "constants/ctes_camera.h"
#include "constants/ctes_object.h"
#include "constants/ctes_bones.h"

//--------------------------------------------------------------------------------------
void VSSkin(
	in float4 iPos : POSITION
	, in float3 iNormal : NORMAL
	, in float2 iTex0 : TEXCOORD0
	, in float4 iTangent : NORMAL1
	, in int4   iBones : BONES
	, in float4 iWeights : WEIGHTS
	, out float4 oPos : SV_POSITION
	, out float3 oNormal : NORMAL0
	, out float4 oTangent : NORMAL1
	, out float2 oTex0 : TEXCOORD0
	, out float3 oWorldPos : TEXCOORD1
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

	//Normal, tangent
	oNormal = mul(iNormal, (float3x3)skin_mtx);
	oTangent.xyz = mul(iTangent.xyz, (float3x3)World);
	oTangent.w = iTangent.w;

	// Las uv's se pasan directamente al ps
	oTex0 = iTex0;
	oWorldPos = skinned_Pos;
}