#ifndef INC_MODULE_ENTITIES_H_
#define INC_MODULE_ENTITIES_H_

#include "app_modules/app_module.h"
#include "handle/handle.h"
#include <vector>
#include <thread>

class CEntitiesModule : public IAppModule {
	void renderInMenu();
	bool use_parallel = true;
	std::thread navmeshThread;
	std::string current_level = "";
	std::string next_level = "";
public:
	std::vector< CHandle > collisionables;
	std::string sala;
	std::string salaloc;

	bool start() override;
	void stop() override;
	void update(float dt) override;

	void initLevel(std::string);
	void clear(std::string next_level = "");
	bool isCleared();
	void destroyRandomEntity(float percent);

	void render() override;
	void recalcNavmesh();
	void readNavmesh();
	bool forcedUpdate() {
		return true;
	}

	static void fixedUpdate(float elapsed);

	const char* getName() const {
		return "entities";
	}

	std::string getCurrentLevel() { return current_level; }

	int size();
};

#endif
