#ifndef INC_COMPONENT_HIERARCHY_H_
#define INC_COMPONENT_HIERARCHY_H_

#include "comp_transform.h"

struct TCompHierarchy : public TCompTransform {
  CHandle h_parent_transform;
  CHandle h_my_transform;
  char    parent_name[64];
  void linkTo(CHandle e);
  bool load(MKeyValue& atts);
  void onGroupCreated(const TMsgEntityGroupCreated& msg);
  void updateWorldFromLocal();
};

#endif

