#ifndef INC_RENDER_RENDER_CONFIG_H_
#define INC_RENDER_RENDER_CONFIG_H_

enum ZConfig {
  ZCFG_DEFAULT
  //, ZCFG_DEPTH_EQUAL
  , ZCFG_ALL_DISABLED
  //, ZCFG_TEST_BUT_NO_WRITE
  //, ZCFG_SHADOWS
, ZCFG_COUNT
};
void activateZ(enum ZConfig);

enum RSConfig {
  RSCFG_DEFAULT
, RSCFG_WIREFRAME
, RSCFG_COUNT
};

void activateRS(enum RSConfig);

// ------------------------------------
void createRenderStateConfigs();
void destroyRenderStateConfigs();

#endif

