#ifndef INC_MODULE_ENTITIES_H_
#define INC_MODULE_ENTITIES_H_

#include "app_modules/app_module.h"
#include "handle/handle.h"
#include <vector>

class CEntitiesModule : public IAppModule {
	void renderInMenu();
	bool use_parallel = true;
public:
	std::vector< CHandle > collisionables;
	std::string sala;
	std::string salaloc;

	bool start() override;
	void stop() override;
	void update(float dt) override;

	void initLevel(std::string);
	void clear();
	bool isCleared();
	void destroyRandomEntity(float percent);

	void render() override;
	void recalcNavmesh();
	void readNavmesh();
	bool forcedUpdate() {
		return true;
	}

	static void fixedUpdate(float elapsed);

	static void fixedUpdate();
	const char* getName() const {
		return "entities";
	}
	int size();
};

#endif
