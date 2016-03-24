#ifndef INC_RESOURCE_SKELETON_H_
#define INC_RESOURCE_SKELETON_H_

#include "resources/resource.h"
#include "utils/XMLParser.h"

class CalCoreModel;

class CSkeleton : public IResource, public CXMLParser {
  CalCoreModel*   core_model;

  void onStartElement(const std::string &elem, MKeyValue &atts);
public:
  CSkeleton();
  eType getType() const { return SKELETON; }

  bool isValid() const {
    return core_model != nullptr;
  }
  void destroy();

  CalCoreModel* getCoreModel() { return core_model; }
};

#endif

