#ifndef INC_CTES_PLATFORM_H_
#define INC_CTES_PLATFORM_H_

#define CTE_SHADER_OBJECT_SLOT     0
#define CTE_SHADER_CAMERA_SLOT     1
#define CTE_SHADER_BONES_SLOT      2


#ifdef INC_MCV_PLATFORM_H_

// VC++ uses this

// The buffer cte is not used by C++
#define ShaderBuffer(name,cte) struct name 
#define matrix    MAT44
#define float4    VEC4

#else

// HLSL uses this definition

#define ShaderBuffer(name,cte) cbuffer name : register(b##cte)

#endif

#endif

