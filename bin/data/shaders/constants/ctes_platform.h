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

//textures temporals, range 50-100
#define TEXTURE_SLOT_DATA1				50
#define TEXTURE_SLOT_DATA2				51

//cubemaps for environment

//101 - 105 -> HUB
#define CUBEMAP_SLOT_HUB1				101

//105 - 112 -> TUTO MOLE
#define CUBEMAP_SLOT_MOLE1				105
#define CUBEMAP_SLOT_MOLE2				106
#define CUBEMAP_SLOT_MOLE3				107
#define CUBEMAP_SLOT_MOLE4				108

//113 - 120 -> TUTO CIENTIFICO
#define CUBEMAP_SLOT_SCI1				113
#define CUBEMAP_SLOT_SCI2				114
#define CUBEMAP_SLOT_SCI3				115
#define CUBEMAP_SLOT_SCI4				116

//121 - 150 -> ROOM1 (SALA GRANDE CAOTICA)
#define CUBEMAP_SLOT_ROOM_1_01				121
#define CUBEMAP_SLOT_ROOM_1_02				122
#define CUBEMAP_SLOT_ROOM_1_03				123
#define CUBEMAP_SLOT_ROOM_1_04				124
#define CUBEMAP_SLOT_ROOM_1_05				125
#define CUBEMAP_SLOT_ROOM_1_06				126
#define CUBEMAP_SLOT_ROOM_1_07				127
#define CUBEMAP_SLOT_ROOM_1_08				128
#define CUBEMAP_SLOT_ROOM_1_09				129
#define CUBEMAP_SLOT_ROOM_1_10				130
#define CUBEMAP_SLOT_ROOM_1_11				131
#define CUBEMAP_SLOT_ROOM_1_12				132
#define CUBEMAP_SLOT_ROOM_1_13				133
#define CUBEMAP_SLOT_ROOM_1_14				134
#define CUBEMAP_SLOT_ROOM_1_15				135
#define CUBEMAP_SLOT_ROOM_1_16				136
#define CUBEMAP_SLOT_ROOM_1_17				137
#define CUBEMAP_SLOT_ROOM_1_18				138
#define CUBEMAP_SLOT_ROOM_1_19				139
#define CUBEMAP_SLOT_ROOM_1_20				140
#define CUBEMAP_SLOT_ROOM_1_21				141
#define CUBEMAP_SLOT_ROOM_1_22				142
#define CUBEMAP_SLOT_ROOM_1_23				143
#define CUBEMAP_SLOT_ROOM_1_24				144
#define CUBEMAP_SLOT_ROOM_1_25				145
#define CUBEMAP_SLOT_ROOM_1_26				146
#define CUBEMAP_SLOT_ROOM_1_27				147

//151 - 180 -> ROOM2 (SALA ARRIBA)
#define CUBEMAP_SLOT_ROOM_2_01				151
#define CUBEMAP_SLOT_ROOM_2_02				152
#define CUBEMAP_SLOT_ROOM_2_03				153
#define CUBEMAP_SLOT_ROOM_2_04				154
#define CUBEMAP_SLOT_ROOM_2_05				155
#define CUBEMAP_SLOT_ROOM_2_06				156
#define CUBEMAP_SLOT_ROOM_2_07				157
#define CUBEMAP_SLOT_ROOM_2_08				158
#define CUBEMAP_SLOT_ROOM_2_09				159
#define CUBEMAP_SLOT_ROOM_2_10				160
#define CUBEMAP_SLOT_ROOM_2_11				161
#define CUBEMAP_SLOT_ROOM_2_12				162
#define CUBEMAP_SLOT_ROOM_2_13				163
#define CUBEMAP_SLOT_ROOM_2_14				164
#define CUBEMAP_SLOT_ROOM_2_15				165
#define CUBEMAP_SLOT_ROOM_2_16				166

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
#define ReadCubeMapHub(nreg) txHubEnv ## nreg

#endif

#endif
