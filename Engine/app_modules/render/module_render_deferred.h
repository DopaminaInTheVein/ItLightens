#ifndef INC_MODULE_RENDER_DEFERRED_H_
#define INC_MODULE_RENDER_DEFERRED_H_

#include "app_modules/app_module.h"
#include "render/render.h"

class CRenderDeferredModule : public IAppModule {

  CRenderToTexture* rt;

public:
  bool start() override;
  void stop() override;
  void render() override;
  const char* getName() const {
    return "render_deferred";
  }
};

#endif
