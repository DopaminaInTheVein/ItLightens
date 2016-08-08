#ifndef INC_MODULE_ENTITIES_H_
#define INC_MODULE_ENTITIES_H_

#include "app_modules/app_module.h"
#include "handle/handle.h"
#include <vector>
#include <thread>

class CEntitiesModule : public IAppModule {
	void renderInMenu();
	bool use_parallel = true;
	bool reloading; // Only clean the reloadable entities
	std::thread navmeshThread;
public:
	std::vector< CHandle > collisionables;
	std::string sala;
	std::string salaloc;

	struct ParsingInfo {
		std::string filename;
		bool reload;
	};

	bool start() override;
	void stop() override;
	void update(float dt) override;

	//void initLevel(std::string, bool check_point, bool reload);
	void initEntities();
	bool loadXML(ParsingInfo& info);
	void saveLevel();
	void clear(bool reload);//std::string next_level = "");
	bool isCleared();
	void destroyRandomEntity(float percent);

	void render() override;
	bool forcedUpdate() {
		return true;
	}

	static void fixedUpdate(float elapsed);

	const char* getName() const {
		return "entities";
	}

	//std::string getCurrentLevel() { return current_level; }

	int size();
};

#endif
