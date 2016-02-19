#ifndef INC_ENTITY_TAGS_H_
#define INC_ENTITY_TAGS_H_

#include <vector>
#include <map>
#include "handle/handle.h"

typedef uint32_t TTagID;
typedef std::vector< CHandle > VHandles;

class TTagsManager : private std::map< TTagID, VHandles > {
public:
  void addTag(CHandle h, TTagID tag_id);
  const VHandles& getHandlesByTag(TTagID tag_id) const;
  CHandle getFirstHavingTag(TTagID tag_id) const;
};

extern TTagsManager tags_manager;

#endif