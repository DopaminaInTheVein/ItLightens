#include "mcv_platform.h"
#include "handle_manager.h"
#include "components\entity_tags.h"

// ------------------------------------
bool CHandle::isValid() const {
	auto hm = CHandleManager::getByType(type);
	return hm && hm->isValid(*this);
}

void CHandle::destroy() {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		hm->destroyHandle(*this);
}

bool CHandle::load(MKeyValue& atts) {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		return hm->load(*this, atts);
	return false;
}

void CHandle::renderInMenu() {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		hm->renderInMenu(*this);
}

// ------------------------------------
void CHandle::setOwner(CHandle new_owner) {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		hm->setOwner(*this, new_owner);
}

CHandle CHandle::getOwner() const {
  auto hm = CHandleManager::getByType(type);
  if (hm)
    return hm->getOwner(*this);
  return CHandle();
}

bool CHandle::hasTag(std::string tag)
{
	std::vector<TTagID> tags;
	tags_manager.getTagFromHandle(*this, tags);
	TTagID tag_id = getID(tag.c_str());
	bool ret = isInVector(tags, tag_id);
	return ret;
}
