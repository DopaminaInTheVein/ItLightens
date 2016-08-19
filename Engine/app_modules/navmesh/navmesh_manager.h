#ifndef INC_NAVMES_MANAGER_H_
#define INC_NAVMES_MANAGER_H_

class CNavmeshManager {
public:
	static void initNavmesh(std::string level_name);
private:
	static void readNavmesh(std::string level_name);
	static void recalcNavmesh(std::string level_name);
	static std::string getPathNavmesh(std::string level_name);
};

extern CGameController* GameController;

#endif