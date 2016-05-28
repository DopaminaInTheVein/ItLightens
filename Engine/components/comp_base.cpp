#include "mcv_platform.h"
#include "comp_base.h"
#include "components/entity.h"

bool TCompBase::getUpdateInfo(CHandle parent) {
	compBaseEntity = parent;
	return parent.isValid();
}