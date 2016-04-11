#ifndef INC_RESOURCE_SKELETON_H_
#define INC_RESOURCE_SKELETON_H_

#include "resources/resource.h"
#include "utils/XMLParser.h"
#include "cal3d/cal3d.h"

#define MAX_NUMBER_ANIMS	32

class CSkeleton : public IResource, public CXMLParser {
	CalCoreModel*   core_model;

	std::string nameAnims[MAX_NUMBER_ANIMS];
	void saveAnimId(std::string src, int anim_id);

	void onStartElement(const std::string &elem, MKeyValue &atts);
public:
	CSkeleton();
	eType getType() const { return SKELETON; }

	bool isValid() const {
		return core_model != nullptr;
	}
	void destroy();

	CalCoreModel* getCoreModel() { return core_model; }
	int getAnimIdByName(std::string name) const;
};

#endif
