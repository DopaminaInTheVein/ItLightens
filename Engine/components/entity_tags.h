#ifndef INC_ENTITY_TAGS_H_
#define INC_ENTITY_TAGS_H_

#include <vector>
#include <map>
#include "handle/handle.h"

typedef uint32_t TTagID;
typedef std::vector< CHandle > VHandles;

CHandle findByName(const VHandles& handles, const char* entity_name);

class TTagsManager : private std::map< TTagID, VHandles > {
  std::map< TTagID, std::string > name_of_tag;
public:
	void addTag(CHandle h, TTagID tag_id);
	void removeTag(CHandle h, TTagID tag_id);
	const VHandles& getHandlesByTag(TTagID tag_id) const;
	const VHandles & getHandlesByTag(std::string tag_id) const;
	CHandle getFirstHavingTag(TTagID tag_id) const;
	CHandle getFirstHavingTag(std::string tag_id) const;
	void getTagFromHandle(CHandle h_match, std::vector<TTagID>& out);

	void renderInMenu();

	const char* getNameOfTag(TTagID tag_id) const;
  	void registerTag(const std::string& tag_name);
};

extern TTagsManager tags_manager;

#endif