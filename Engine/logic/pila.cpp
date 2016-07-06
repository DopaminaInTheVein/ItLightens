#include "mcv_platform.h"
#include "pila.h"

#include "components/comp_physics.h"
#include "components/entity.h"
#include "components/comp_transform.h"

#define ST_PILA_GROUND "on_ground"
#define ST_PILA_GRABBED "grabbed"
#define ST_PILA_FALLING "falling"

using namespace std;

VHandles TCompPila::all_pilas;
map<string, statehandler> TCompPila::statemap = {};

void TCompPila::setFalling()
{
	GET_MY(phys, TCompPhysics);
	phys->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, true);
	ChangeState(ST_PILA_FALLING);
}

#define AddStPila(name, func) AddState(name, (statehandler)&TCompPila::func)

void TCompPila::onCreate(const TMsgEntityCreated& msg)
{
	all_pilas.push_back(CHandle(this).getOwner());
	GET_MY(phys, TCompPhysics);

	AddStPila(ST_PILA_GROUND, OnGround);
	AddStPila(ST_PILA_GRABBED, Grabbed);
	AddStPila(ST_PILA_FALLING, Falling);
	ChangeState(ST_PILA_GROUND);
}

void TCompPila::update(float elapsed) {
	Recalc();
	//updateFalling(elapsed);
}

TCompPhysics* TCompPila::keepVertical() {
	GET_MY(phys, TCompPhysics);
	GET_MY(tmx, TCompTransform);
	tmx->setPitch(0.f);
	phys->setPosition(tmx);
	return phys;
}

void TCompPila::OnGround()
{
	keepVertical();
}
void TCompPila::Falling()
{
	auto phys = keepVertical();
	phys->AddMovement(VEC3(0.f, getDeltaTime() * -10.f, 0.f));
}
void TCompPila::Grabbed()
{
}
//void TCompPila::updateFalling(float elapsed) {
//	if (falling) {
//		GET_MY(phys, TCompPhysics);
//		GET_MY(tmx, TCompTransform);
//		tmx->setPitch(0.f);
//		phys->setPosition(tmx);
//		phys->AddMovement(VEC3(0.f, elapsed * -10.f, 0.f));
//	}
//}

void TCompPila::onContact(const TMsgContact& msg)
{
	GET_MY(phys, TCompPhysics);
	phys->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);
	ChangeState(ST_PILA_GROUND);
}

bool TCompPila::load(MKeyValue& atts) {
	return true;
}

TCompPila::~TCompPila() {
	removeFromVector(all_pilas, CHandle(this).getOwner());
}