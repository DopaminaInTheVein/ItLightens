#include "mcv_platform.h"
#include "handle_manager.h"
#include "components\entity_tags.h"
#include "components\entity.h"

#include <fstream>
// ------------------------------------
bool ClHandle::isValid() const {
	auto hm = CHandleManager::getByType(type);
	return hm && hm->isValid(*this);
}

void ClHandle::destroy() {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		hm->destroyHandle(*this);
}

bool ClHandle::load(MKeyValue& atts) {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		return hm->load(*this, atts);
	return false;
}

bool ClHandle::save(std::ofstream& os, MKeyValue& atts) {
	auto hm = CHandleManager::getByType(type);
	if (hm)	return hm->save(*this, os, atts);
	return false;
}

void ClHandle::renderInMenu() {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		hm->renderInMenu(*this);
}

// ------------------------------------
void ClHandle::setOwner(ClHandle new_owner) {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		hm->setOwner(*this, new_owner);
}

ClHandle ClHandle::getOwner() const {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		return hm->getOwner(*this);
	return ClHandle();
}

bool ClHandle::hasTag(std::string tag)
{
	CEntity* e = *this;
	if (e) {
		return e->hasTag(tag);
	}
	else {
		ClHandle h = this->getOwner();
		if (h.isValid()) {
			e = h;
			if (e) {
				return e->hasTag(tag);
			}
		}
	}

	return false;
}