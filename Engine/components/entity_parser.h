#ifndef INC_ENTITY_PARSER_H_
#define INC_ENTITY_PARSER_H_

#include "utils/XMLParser.h"
#include "handle/handle.h"

class CEntityParser : public CXMLParser {
	CHandle curr_entity;
public:
	void onStartElement(const std::string &elem, MKeyValue &atts) override;
	void onEndElement(const std::string &elem) override;
};

#endif
