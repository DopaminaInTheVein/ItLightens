#include "mcv_platform.h"
#include "entity_parser.h"
#include "handle/handle_manager.h"
#include "components/entity.h"
#include "components/comp_msgs.h"
#include "components/entity_tags.h"

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

std::map< std::string, CPrefabCompiler* > compiled_prefabs;

CHandle createPrefab(const std::string& prefab) {
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


void CEntityParser::onStartElement(const std::string &elem, MKeyValue &atts) {

  if (curr_prefab_compiler) {
    CPrefabCompiler::TCall c;
    c.is_start = true;
    c.elem = elem;
    c.atts = atts;
    curr_prefab_compiler->calls.push_back(c);
  }

  if (elem == "entities") {
    return;
  }

  CHandle new_h;
  bool    reusing_component = false;

  // Check if we find a new entity with the prefab attr
  if (elem == "entity") {
    auto prefab = atts["prefab"];
    if (!prefab.empty()) 
      new_h = createPrefab(prefab);
  }

  // Inside an entity...?
  if (curr_entity.isValid()) {
    auto hm = CHandleManager::getByName(elem.c_str());
    // Check if the current entity already has this type
    // of component...
    CEntity* e = curr_entity;
    new_h = e->getByCompIndex(hm->getType());
    reusing_component = new_h.isValid();
  }

  // If not prefab has been generated... create one of the 
  // type of the tag
  if (!new_h.isValid()) {
    auto hm = CHandleManager::getByName(elem.c_str());
    new_h = hm->createHandle();
  }

  if (elem == "entity") {
    curr_entity = new_h;
  }
  // Estoy parseando un component
  else {
    assert(curr_entity.isValid());
    CEntity* e = curr_entity;
    new_h.load(atts);
    if( !reusing_component )
      e->add(new_h);
  }
}

void CEntityParser::onEndElement(const std::string &elem)  {

  if (curr_prefab_compiler) {
    CPrefabCompiler::TCall c;
    c.is_start = false;
    c.elem = elem;
    curr_prefab_compiler->calls.push_back(c);
  }

  //dbg("Bye from %s\n", elem.c_str());
  if (elem == "entity") {
    handles.push_back(curr_entity);
    // Keep track of the first entity found in the file
    if (!root_entity.isValid())
      root_entity = curr_entity;
    curr_entity = CHandle();
  }

  if (elem == "entities" || elem == "prefab") {
    for (auto h : handles) {
      CEntity*e = h;
      dbg("Entity created!!\n");
      e->sendMsg(TMsgEntityCreated());
    }

    // Send the group has been created msg
    TMsgEntityGroupCreated msg = { &handles };
    for (auto h : handles)
      ((CEntity*)h)->sendMsg(msg);
  }
}


