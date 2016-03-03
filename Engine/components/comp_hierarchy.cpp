#include "mcv_platform.h"
#include "comp_hierarchy.h"

bool TCompHierarchy::load(MKeyValue& atts) {
  TCompTransform::load(atts);
  // read the parent
  return true;
}

void TCompHierarchy::updateWorldFromLocal() {

}

