#ifndef INC_COMPONENT_TAGS_H_
#define INC_COMPONENT_TAGS_H_

#include "comp_base.h"
#include "entity_tags.h"
#include <sstream>

typedef uint32_t TTagID;

template< typename CB >
void split(const std::string& str, CB cb) {
  std::string word;
  std::istringstream iss(str, std::istringstream::in);
  while (iss >> word)
    cb(word);
}

// ----------------------------------------------------------------
struct TCompTags : public TCompBase {
  static const uint32_t max_tags = 4;
  TTagID   tags[max_tags];

  TCompTags() {
    for (uint32_t i = 0; i < max_tags; ++i)
      tags[i] = 0x00;
  }

  bool load(MKeyValue& atts) {
    auto all_tags = atts.getString("tags", "");
    int n = 0;
    split(all_tags, [this,&n](const std::string& word) {
      auto tag_id = getID(word.c_str());
      tags_manager.registerTag(word);
      tags[n++] = tag_id;
    });
    return true;
  }

  bool created = false;
  void onCreate(const TMsgEntityCreated&) {
	if (created) return;
	created = true;
    CHandle h_entity = CHandle(this).getOwner();
    for(uint32_t i=0; i<max_tags; ++i)
      if( tags[i] )
        tags_manager.addTag(h_entity, tags[i]);
  }

  bool hasTag(TTagID tag) {
	  for (auto t : tags) {
		  if (t == tag)
			  return true;
	  }

	  return false;
  }

  void renderInMenu() {
    for (uint32_t i = 0; i < max_tags; ++i) {
      if(tags[i])
        ImGui::Text("Tag %d : %s %08x", i, tags_manager.getNameOfTag( tags[ i ] ), tags[i] );
    }
  }

  void onTagAdded( const TMsgAddTag& msg) {
    CHandle h_entity = CHandle(this).getOwner();
    for (int i = 0; i < max_tags; ++i) {
      // Already found?
      if (tags[i] == msg.tag_id)
        return;
      // Once we found an empty slot, fill it
      if (!tags[i]) {
        tags[i] = msg.tag_id;
        tags_manager.addTag(h_entity, msg.tag_id);
        break;
      }
    }
  }
};

#endif

