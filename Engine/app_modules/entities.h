#ifndef INC_MODULE_ENTITIES_H_
#define INC_MODULE_ENTITIES_H_

#include "app_modules/app_module.h"

class CEntitiesModule : public IAppModule {
  void renderInMenu();
  void orbitCamera(float angle);

public:
  bool start() override;
  void stop() override;
  void update(float dt) override;
  void render() override;
  const char* getName() const {
    return "entities";
  }
};

#endif
