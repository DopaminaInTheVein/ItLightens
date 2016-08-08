#ifndef INC_ENTITY_PARSER_H_
#define INC_ENTITY_PARSER_H_

#include "handle/handle.h"
#include "components/entity_tags.h"
#include <set>

class CPrefabCompiler {
public:
	struct TCall {
		bool        is_start;
		std::string elem;
		MKeyValue   atts;
	};
	std::vector< TCall > calls;
	void execute(CXMLParser* parser) {
		for (auto& c : calls) {
			if (c.is_start)
				parser->onStartElement(c.elem, c.atts);
			else
				parser->onEndElement(c.elem);
		}
	}
};

namespace IdEntities {
	void init();
	CHandle findById(const int entity_id);
	void saveIdEntity(const CHandle entity, const int entity_id);
}

class CEntityParser : public CXMLParser {
	CHandle curr_entity;
	int curr_entity_id;
	bool curr_entity_permanent;
	bool curr_entity_reload;
	bool curr_entity_slept;
	bool first_load;
	bool reload = false;
	CHandle root_entity;
	VHandles handles;
	static VHandles collisionables;
	CPrefabCompiler* curr_prefab_compiler;
	CPrefabCompiler* curr_slept_compiler;
	static std::set<std::string> loaded_files;
public:
	CEntityParser() : curr_prefab_compiler(nullptr) {}// { IdEntities::init(); }
	CEntityParser(bool new_reload) : curr_prefab_compiler(nullptr), reload(new_reload) {}// { IdEntities::init(); }
	CEntityParser(CHandle parent) { curr_entity = root_entity = parent; }
	CHandle getRootEntity() { return root_entity; }
	void onStartElement(const std::string &elem, MKeyValue &atts) override;
	void onEndElement(const std::string &elem) override;

	static const std::vector<CHandle> CEntityParser::getCollisionables() {
		return collisionables;
	}
	static void clearCollisionables() {
		collisionables.clear();
	}

	void setPrefabCompiler(CPrefabCompiler* new_prefab_compiler) {
		curr_prefab_compiler = new_prefab_compiler;
	}
	bool xmlParseFile(const std::string &filename) override {
		first_load = loaded_files.find(filename) == loaded_files.end();
		bool result = CXMLParser::xmlParseFile(filename);
		loaded_files.insert(filename);
		return result;
	}
	bool hasToCreate();
};

CHandle spawnPrefab(const std::string& prefab); // create Prefab and call onCreate
CHandle createPrefab(const std::string& prefab);

#endif
