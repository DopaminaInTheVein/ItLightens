#ifndef INC_ENTITY_TAGS_H_
#define INC_ENTITY_TAGS_H_

#include <vector>
#include <map>
#include "handle/handle.h"

typedef uint32_t TTagID;
typedef std::vector< ClHandle > VHandles;

ClHandle findByName(const VHandles& handles, const char* entity_name);

class TTagsManager : private std::map< TTagID, VHandles > {
	std::map< TTagID, std::string > name_of_tag;
public:
	void addTag(ClHandle h, TTagID tag_id);
	void removeTag(ClHandle h, TTagID tag_id);
	void removeAllTags(ClHandle h);
	const VHandles& getHandlesByTag(TTagID tag_id) const;
	const VHandles & getHandlesByTag(std::string tag_id) const;
	ClHandle getFirstHavingTag(TTagID tag_id) const;
	ClHandle getFirstHavingTag(std::string tag_id) const;
	void getTagFromHandle(ClHandle h_match, std::vector<TTagID>& out);
	ClHandle getHandleByTagAndName(const char* name, const char* tag) const;

	void renderInMenu();

	const char* getNameOfTag(TTagID tag_id) const;
	void registerTag(const std::string& tag_name);
};

extern TTagsManager tags_manager;

#endif