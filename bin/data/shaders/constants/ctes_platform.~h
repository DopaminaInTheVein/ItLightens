#ifndef INC_CTES_PLATFORM_H_
#define INC_CTES_PLATFORM_H_

#define CTE_SHADER_OBJECT_SLOT     0
#define CTE_SHADER_CAMERA_SLOT     1
#define CTE_SHADER_BONES_SLOT      2
#define CTE_SHADER_LIGHT           3
#define CTE_SHADER_GLOBALS_SLOT    4

#define TEXTURE_SLOT_DIFFUSE       0
#define TEXTURE_SLOT_NORMALS       1
#define TEXTURE_SLOT_SPECULAR      2
#define TEXTURE_SLOT_SELFILUM      3
#define TEXTURE_SLOT_SHADOWMAP     4

#define TEXTURE_SLOT_WORLD_POS     5
#define TEXTURE_SLOT_ENVIRONMENT   6
#define TEXTURE_SLOT_LIGHT_MASK    7
#define TEXTURE_SLOT_NOISE         8


#ifdef INC_MCV_PLATFORM_H_

// VC++ uses this

// The buffer cte is not used by C++
#define ShaderBuffer(name,cte) struct name
#define matrix    MAT44
#define float4    VEC4
#define USE_SHADER_REG(nreg)

#else

// HLSL uses this definition

#define ShaderBuffer(name,cte) cbuffer name : register(b##cte)
#define USE_SHADER_REG(nreg) register(t ## nreg)

#endif

#endif
