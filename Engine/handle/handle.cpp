#include "mcv_platform.h"
#include "handle_manager.h"
#include "components\entity_tags.h"
#include "components\entity.h"

#include <fstream>
// ------------------------------------
bool CHandle::isValid() const {
	auto hm = CHandleManager::getByType(type);
	return hm && hm->isValid(*this);
}

void CHandle::destroy() {
	if (!isValid()) return;
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

bool CHandle::save(std::ofstream& os, MKeyValue& atts) {
	auto hm = CHandleManager::getByType(type);
	if (hm)	return hm->save(*this, os, atts);
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
	CEntity* e = *this;
	if (e) {
		return e->hasTag(tag);
	}
	else {
		CHandle h = this->getOwner();
		if (h.isValid()) {
			e = h;
			if (e) {
				return e->hasTag(tag);
			}
		}
	}

	return false;
}