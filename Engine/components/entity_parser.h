#ifndef INC_ENTITY_PARSER_H_
#define INC_ENTITY_PARSER_H_

#include "utils/XMLParser.h"
#include "handle/handle.h"
#include <vector>

class CPrefabCompiler;

class CEntityParser : public CXMLParser {
  CHandle curr_entity;
  CHandle root_entity;
  CPrefabCompiler* curr_prefab_compiler;
  std::vector< CHandle > collisionables;
public:
  CEntityParser() : curr_prefab_compiler( nullptr ) { }
  CHandle getRootEntity() { return root_entity; }
  void onStartElement(const std::string &elem, MKeyValue &atts) override;
  void onEndElement(const std::string &elem) override;
  void setPrefabCompiler(CPrefabCompiler* new_prefab_compiler) {
    curr_prefab_compiler = new_prefab_compiler;
  }
};

CHandle createPrefab(const std::string& prefab);

#endif

