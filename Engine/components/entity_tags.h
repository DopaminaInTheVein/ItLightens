#ifndef INC_ENTITY_TAGS_H_
#define INC_ENTITY_TAGS_H_

#include <vector>
#include <map>
#include "handle/handle.h"
#include "components/entity.h"

typedef uint32_t TTagID;
typedef std::vector< CHandle > VHandles;
typedef std::vector< CEntity * > VEntities;

class TTagsManager : private std::map< TTagID, VHandles > {
public:
	void addTag(CHandle h, TTagID tag_id);
	void removeTag(CHandle h, TTagID tag_id);
	const VHandles& getHandlesByTag(TTagID tag_id) const;
	VEntities getHandlesPointerByTag(TTagID tag_id);
	CHandle getFirstHavingTag(TTagID tag_id) const;
};

extern TTagsManager tags_manager;

#endif