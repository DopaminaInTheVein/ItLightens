#ifndef INC_ENTITY_PARSER_H_
#define INC_ENTITY_PARSER_H_

#include "utils/XMLParser.h"
#include "handle/handle.h"
#include <vector>

class CEntityParser : public CXMLParser {
	CHandle curr_entity;
	std::vector< CHandle > collisionables;

public:
	void onStartElement(const std::string &elem, MKeyValue &atts) override;
	void onEndElement(const std::string &elem) override;
	std::vector< CHandle > getCollisionables();
};

#endif
