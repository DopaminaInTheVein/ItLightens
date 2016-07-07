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

	mActionable = true;
	mAction = EXAMINATE;
	mActionSci = EXAMINATE;
	mActionMole = NONE;

	AddStPila(ST_PILA_GROUND, OnGround);
	AddStPila(ST_PILA_GRABBED, Grabbed);
	AddStPila(ST_PILA_FALLING, Falling);

	place = VEC3();
	ChangeState(ST_PILA_GROUND);
}

void TCompPila::update(float elapsed) {
	Recalc();
	//updateFalling(elapsed);
}

void TCompPila::OnGround()
{
	keepVertical();
	checkActions();
	if (!isZero(place)) {
		GET_MY(tmx, TCompTransform);
		GET_MY(phys, TCompPhysics);
		tmx->setPosition(place);
		phys->setPosition(tmx);
	}
}

void TCompPila::Grabbed()
{
}

void TCompPila::Falling()
{
	auto phys = keepVertical();
	phys->AddMovement(VEC3(0.f, getDeltaTime() * -10.f, 0.f));
}

void TCompPila::onContact(const TMsgContact& msg)
{
	GET_MY(phys, TCompPhysics);
	phys->setBehaviour(PHYS_BEHAVIOUR::eUSER_CALLBACK, false);
	ChangeState(ST_PILA_GROUND);
}

TCompPhysics* TCompPila::keepVertical() {
	GET_MY(phys, TCompPhysics);
	GET_MY(tmx, TCompTransform);
	tmx->setPitch(0.f);
	phys->setPosition(tmx);
	return phys;
}

void TCompPila::checkActions()
{
	if (!isPlayerNear()) return;
	onTriggerInside(TMsgTriggerIn());
}

eAction TCompPila::getActionAvailable()
{
	if (!isZero(place)) return NONE;
	//Resultado
	eAction action = NONE;

	TMsgGetWhoAmI msg;
	player.sendMsgWithReply(msg);
	playerType = msg.who;

	switch (playerType) {
	case PLAYER_TYPE::PLAYER:
		action = mAction;
		break;
	case PLAYER_TYPE::MOLE:
		action = mActionMole;
		break;
	case PLAYER_TYPE::SCIENTIST:
		action = mActionSci;
		break;
	}
	return action;
}

void TCompPila::executeTrigger(CLogicManagerModule::EVENT logicEvent) { //, CHandle handle) {
	CEntity* eMe = CHandle(this).getOwner();
	logic_manager->throwEvent(logicEvent, string(eMe->getName()), CHandle(this).getOwner());
}

bool TCompPila::getPlayer()
{
	if (player.isValid()) return true;
	player = tags_manager.getFirstHavingTag("player");
	return player.isValid();
}

bool TCompPila::isPlayerNear()
{
	if (!getPlayer()) return false;
	GET_COMP(tmx_player, player, TCompTransform);
	GET_MY(tmx, TCompTransform);
	return inSquaredRangeXZ_Y(tmx->getPosition(), tmx_player->getPosition(), 4.f, 5.f);
}

bool TCompPila::load(MKeyValue& atts) {
	return true;
}

TCompPila::~TCompPila() {
	removeFromVector(all_pilas, CHandle(this).getOwner());
}