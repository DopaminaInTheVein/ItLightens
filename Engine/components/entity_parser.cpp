#include "mcv_platform.h"
#include "entity_parser.h"
#include "handle/handle_manager.h"
#include "components/entity.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"

void CEntityParser::onStartElement(const std::string &elem, MKeyValue &atts) {
	if (elem == "entities") {
		return;
	}
	else if (elem == "tag") {
		auto tag_name = atts.getString("name", "");
		assert(!tag_name.empty());
		tags_manager.addTag(curr_entity, getID(tag_name.c_str()));
		return;
	}

	auto hm = CHandleManager::getByName(elem.c_str());
	CHandle new_h = hm->createHandle();

	if (elem == "entity") {
		curr_entity = new_h;
	}
	// Estoy parseando un component
	else {
		assert(curr_entity.isValid());
		CEntity* e = curr_entity;
		new_h.load(atts);
		e->add(new_h);
		if (elem == "rigidbody") {
			collisionables.push_back(curr_entity);
		}
	}
}

void CEntityParser::onEndElement(const std::string &elem) {
	//dbg("Bye from %s\n", elem.c_str());
	if (elem == "entity") {
		CEntity*e = curr_entity;
		e->sendMsg(TMsgEntityCreated());
		dbg("Entity created!!\n");
		curr_entity = CHandle();
	}
}

std::vector< CHandle > CEntityParser::getCollisionables() {
	return collisionables;
}