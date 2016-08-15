#include "mcv_platform.h"
#include "entity_parser.h"
#include "handle/handle_manager.h"
#include "components/entity.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"

using namespace std;

map< string, CPrefabCompiler* > compiled_prefabs;
set<string> CEntityParser::loaded_files = set<string>();
VHandles CEntityParser::collisionables = VHandles();
// Identified Entities
static VHandles identified_entities;
ClHandle IdEntities::findById(const int entity_id) {
	return identified_entities[entity_id];
}
void IdEntities::init() {
	identified_entities.resize(MAX_ENTITIES);
}
void IdEntities::saveIdEntity(ClHandle entity, int entity_id) {
	if (entity_id >= 0) {
		identified_entities[entity_id] = entity;
		CEntity* e = entity;
		e->setId(entity_id);
	}
}

ClHandle spawnPrefab(const std::string& prefab) {
	ClHandle h = createPrefab(prefab);

	//Le avisamos que se ha creado
	CEntity* e = h;
	TMsgEntityCreated msg;
	e->sendMsg(msg);

	//Devolvemos handle de la entidad creada
	return h;
}

ClHandle createPrefab(const std::string& prefab) {
	// Check if the prefabs is already 'compiled'
	CPrefabCompiler* prefab_compiler = nullptr;
	auto it = compiled_prefabs.find(prefab);
	if (it == compiled_prefabs.end()) {
		// Compile it
		prefab_compiler = new CPrefabCompiler;
		CEntityParser ep;
		ep.setPrefabCompiler(prefab_compiler);
		std::string prefab_filename = "data/prefabs/" + prefab + ".prefab";
		// This will 'CREATE' the entities in the prefab
		bool is_ok = ep.xmlParseFile(prefab_filename);
		assert(is_ok);

		// And register it
		compiled_prefabs[prefab] = prefab_compiler;

		// To be able to keep adding components to the
		// just parsed entity
		return ep.getRootEntity();
	}
	else {
		prefab_compiler = it->second;
		assert(prefab_compiler);
		// Save current root_entity because the we needed it
		// to compile the prefab
		CEntityParser ep;
		// We want to keep track of the root entity found
		// while compiling the prefab
		prefab_compiler->execute(&ep);
		// To be able to keep adding components to the
		// just created with prefab entity
		return ep.getRootEntity();
	}
}

#define MUST_ADD_COMPONENT (!curr_slept_compiler || elem == "entity" || elem == "tags" || elem == "name")
#define MUST_COMPILE_SNOOZER (curr_slept_compiler && elem != "entity" && elem != "tags" && elem != "name")

void CEntityParser::onStartElement(const std::string &elem, MKeyValue &atts) {
	if (curr_prefab_compiler) {
		CPrefabCompiler::TCall c;
		c.is_start = true;
		c.elem = elem;
		c.atts = atts;
		curr_prefab_compiler->calls.push_back(c);
	}
	else if (curr_slept_compiler && elem != "tags") {
		CPrefabCompiler::TCall c;
		c.is_start = true;
		c.elem = elem;
		c.atts = atts;
		curr_slept_compiler->calls.push_back(c);
	}

	if (elem == "entities") {
		return;
	}

	ClHandle new_h;
	bool    reusing_component = false;

	// Check if we find a new entity with the prefab attr
	if (elem == "entity") {
		curr_entity_permanent = atts.getBool("permanent", false);
		curr_entity_reload = atts.getBool("reload", false);

		if (!hasToCreate()) {
			curr_entity = ClHandle();
			return;
		}
		else {
			curr_entity_id = atts.getInt("id", -1);
			curr_entity_slept = atts.getBool("slept", false);
			if (curr_entity_slept) curr_slept_compiler = new CPrefabCompiler;
			auto prefab = atts["prefab"];
			if (!prefab.empty()) new_h = createPrefab(prefab);
		}
	}
	if (!hasToCreate()) return;

	// Inside an entity...?
	if (curr_entity.isValid()) {
		auto hm = CHandleManager::getByName(elem.c_str());
		// Check if the current entity already has this type
		// of component...
		CEntity* e = curr_entity;
		IdEntities::saveIdEntity(ClHandle(e), curr_entity_id);
		e->setPermanent(curr_entity_permanent);
		e->setReload(curr_entity_reload);
		new_h = e->getByCompIndex(hm->getType());
		reusing_component = new_h.isValid();
	}

	// If not prefab has been generated... create one of the
	// type of the tag
	if (!new_h.isValid()) {
		if (MUST_ADD_COMPONENT) {
			auto hm = CHandleManager::getByName(elem.c_str());
			new_h = hm->createHandle();
		}
	}

	if (elem == "entity") {
		curr_entity = new_h;
	}
	// Estoy parseando un component
	else {
		assert(curr_entity.isValid());
		CEntity* e = curr_entity;
		new_h.load(atts);

		if (!curr_slept_compiler && elem == "rigidbody" && atts["type_collision"] == "static") {
			collisionables.push_back(curr_entity);
		}
		if (!reusing_component && MUST_ADD_COMPONENT) {
			e->add(new_h);
		}
	}
}

void CEntityParser::onEndElement(const std::string &elem) {
	if (curr_prefab_compiler) {
		CPrefabCompiler::TCall c;
		c.is_start = false;
		c.elem = elem;
		curr_prefab_compiler->calls.push_back(c);
	}
	if (MUST_COMPILE_SNOOZER) {
		CPrefabCompiler::TCall c;
		c.is_start = false;
		c.elem = elem;
		curr_slept_compiler->calls.push_back(c);
	}

	//dbg("Bye from %s\n", elem.c_str());
	if (elem == "entity") {
		if (!curr_entity.isValid()) return;
		handles.push_back(curr_entity);

		if (curr_slept_compiler) {
			auto hmSnoozer = CHandleManager::getByName("snoozer");
			ClHandle snoozer = hmSnoozer->createHandle();
			CEntity * e = curr_entity;
			e->add(snoozer);
			TMsgPreload msgPreload;
			msgPreload.comps = curr_slept_compiler;
			e->sendMsg(msgPreload);
		}
		// Keep track of the first entity found in the file
		if (!root_entity.isValid())
			root_entity = curr_entity;
		curr_entity = ClHandle();
		curr_slept_compiler = nullptr;
	}

	if (elem == "entities" || elem == "prefab") {
		for (auto h : handles) {
			CEntity*e = h;
			dbg("Entity created!! [%s]\n", e->getName());
			e->sendMsg(TMsgEntityCreated());
		}

		// Send the group has been created msg
		TMsgEntityGroupCreated msg = { &handles };

		for (auto h : handles)
			((CEntity*)h)->sendMsg(msg);
	}
}

bool CEntityParser::hasToCreate()
{
	if (curr_entity_permanent && !first_load) return false;
	if (reload && !curr_entity_reload) return false;
	return true;
}