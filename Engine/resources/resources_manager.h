#ifndef INC_RESOURCES_MANAGER_H_
#define INC_RESOURCES_MANAGER_H_

#include "resource.h"

// ---------------------------------
template< class TObj >
IResource* createObjFromName(const std::string& aname);

// ---------------------------------
class CResourcesManager {
	std::map< std::string, IResource* > all;
public:
	CResourcesManager() { }
	void destroy() {
		for (auto it : all) {
			it.second->destroy();
			delete it.second;
		}
		all.clear();
	}

	const IResource* get(const char* name);

	bool isValid(const char* name) const {
		return all.find(name) != all.end();
	}

	// The manager takes ownership of new_res
  	void registerNew(IResource* new_res);	
	void renderUIDebug(ImGuiTextFilter * filter = nullptr);
};

extern CResourcesManager Resources;

#endif
