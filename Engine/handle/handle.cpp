#include "mcv_platform.h"
#include "handle_manager.h"

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

CHandle CHandle::getOwner() {
	auto hm = CHandleManager::getByType(type);
	if (hm)
		return hm->getOwner(*this);
	return CHandle();
}

/*
 void test_handles() {
   CHandle h_e;
   h_e.create<CEntity>();
   CEntity* e = h_e;

   //CHandle new_life = e->add("life");
   CHandle h0 = CHandleManager::getByName("life")->createHandle();

   CHandle he_life;
   he_life.create<TCompLife>();
   e->add(he_life);
   TCompLife* life = e->get<TCompLife>();
   e->del<TCompLife>();

  CHandle h1;
  h1.create<TCompLife>();
  CHandle h2;
  h2.create<TCompLife>();
  CHandle h3;
  h3.create<TCompLife>();

  assert(h1.isValid());
  assert(h2.isValid());
  assert(h3.isValid());
  TCompLife* life1 = h1;
  TCompLife* life2 = h2;
  TCompLife* life3 = h3;
  life1->life = 1.01f;
  life2->life = 2.71f;
  life3->life = 3.14f;

  CHandle h1b(life1);
  assert(h1b == h1);

  getHandleManager<TCompLife>()->dumpInternals();

  h2.destroy();
  assert(!h2.isValid());
  assert(h1.isValid());
  assert(h3.isValid());

  getHandleManager<TCompLife>()->dumpInternals();

  TCompLife* life2b = h2;
  assert(life2b == nullptr);
  //assert(life1->life == 1.01f);
  //assert(life3->life == 3.44f);
}
 */