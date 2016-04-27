#ifndef INC_RENDER_RENDER_CONFIG_H_
#define INC_RENDER_RENDER_CONFIG_H_

enum ZConfig {
	ZCFG_DEFAULT
	//, ZCFG_DEPTH_EQUAL
	, ZCFG_ALL_DISABLED
	, ZCFG_TEST_BUT_NO_WRITE
	, ZCFG_LIGHTS_CONFIG
  , ZCFG_LIGHTS_DIR_CONFIG
	//, ZCFG_SHADOWS
	, ZCFG_COUNT
};
void activateZ(enum ZConfig);

enum RSConfig {
	RSCFG_DEFAULT
	//	, RSCFG_WIREFRAME
	, RSCFG_INVERT_CULLING
	, RSCFG_COUNT
};
void activateRS(enum RSConfig);

enum BlendConfig {
	BLENDCFG_DEFAULT
	, BLENDCFG_ADDITIVE
	, BLENDCFG_COMBINATIVE
	, BLENDCFG_COUNT
};
void activateBlend(enum BlendConfig);

void activateSamplerStates();

// ------------------------------------
void createRenderStateConfigs();
void destroyRenderStateConfigs();

#endif
