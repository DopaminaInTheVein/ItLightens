#ifndef INC_CTES_PLATFORM_H_
#define INC_CTES_PLATFORM_H_

//Shader constants
#define CTE_SHADER_OBJECT_SLOT			0
#define CTE_SHADER_CAMERA_SLOT			1
#define CTE_SHADER_BONES_SLOT			2
#define CTE_SHADER_LIGHT				3
#define CTE_SHADER_GLOBALS_SLOT			4
#define CTE_SHADER_BLUR_SLOT			5
#define CTE_SHADER_HATCHING_SLOT		6
#define CTE_SHADER_GUI_SLOT				7
#define CTE_SHADER_DREAM_SLOT			8
#define CTE_SHADER_FOG_SLOT				9

//textures object, 0-19
#define TEXTURE_SLOT_DIFFUSE			0
#define TEXTURE_SLOT_NORMALS			1
#define TEXTURE_SLOT_SPECULAR			2
#define TEXTURE_SLOT_SELFILUM			3
#define TEXTURE_SLOT_PLUS				4
#define TEXTURE_SLOT_MINUS				5
#define TEXTURE_SLOT_GLOSS				6


//texture globals, range 10-39
#define TEXTURE_SLOT_SHADOWMAP			10
#define TEXTURE_SLOT_DEPTHS				11
#define TEXTURE_SLOT_ENVIRONMENT		12
#define TEXTURE_SLOT_LIGHT_MASK			13
#define TEXTURE_SLOT_NOISE				14
#define TEXTURE_SLOT_HATCHING			15
#define TEXTURE_SLOT_SPECULAR_GL		16
#define TEXTURE_SLOT_RAMP				17
#define TEXTURE_SLOT_SHADOWS			18
#define TEXTURE_SLOT_FADE_SCREEN		19
#define TEXTURE_SLOT_SPECULAR_LIGHTS	20
#define TEXTURE_SLOT_GLOSSINESS			21
#define TEXTURE_SLOT_SHADOWMAP_STATICS	22


//#ifdef _DEBUG
//texture tests, range 40-49
#define TEXTURE_SLOT_HATCHING_TEST1		95
#define TEXTURE_SLOT_HATCHING_TEST2		96

//#endif

//textures temporals, range 50+
#define TEXTURE_SLOT_DATA1				50
#define TEXTURE_SLOT_DATA2				51


#ifdef INC_MCV_PLATFORM_H_

// VC++ uses this

// The buffer cte is not used by C++
#define ShaderBuffer(name,cte) struct name
#define matrix    MAT44
#define float4    VEC4
#define float2    VEC2
#define USE_SHADER_REG(nreg)

#else

// HLSL uses this definition

#define ShaderBuffer(name,cte) cbuffer name : register(b##cte)
#define USE_SHADER_REG(nreg) register(t ## nreg)

#endif

#endif
