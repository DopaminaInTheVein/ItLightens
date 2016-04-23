#ifndef INC_MODULE_ENTITIES_H_
#define INC_MODULE_ENTITIES_H_

#include "app_modules/app_module.h"
#include "handle/handle.h"
#include <vector>

class CEntitiesModule : public IAppModule {
	void renderInMenu();
	bool use_parallel = false;
public:
	std::vector< CHandle > collisionables;
	std::string sala;
	std::string salaloc;
	std::string salalocExtra;

	bool start() override;
	void stop() override;
	void update(float dt) override;
	void render() override;
	void recalcNavmesh();
	void readNavmesh();
	const char* getName() const {
		return "entities";
	}
};

#endif
