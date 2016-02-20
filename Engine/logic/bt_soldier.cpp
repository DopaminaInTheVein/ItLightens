#include "bt_soldier.h"


void bt_soldier::create(string s, TEntity* new_entity)
{
name=s;
createRoot("patrullero", PRIORITY, NULL, NULL);
//nombrePadre, miNombre, tipoQNodo, Condicion, Accion
addChild("patrullero", "escape", ACTION, (btcondition)&bt_soldier::conditionReset, (btaction)&bt_soldier::actionReset);
addChild("patrullero", "fight", RANDOM, (btcondition)&bt_soldier::conditionFight, NULL);
addChild("patrullero", "chase", ACTION, (btcondition)&bt_soldier::conditionChase, (btaction)&bt_soldier::actionChase);
addChild("patrullero", "patrol", SEQUENCE, (btcondition)&bt_soldier::conditionPatrol, NULL);
addChild("fight", "combat", ACTION, NULL, (btaction)&bt_soldier::actionCombat);
addChild("fight", "o_left", ACTION, NULL, (btaction)&bt_soldier::actionOLeft);
addChild("fight", "o_right", ACTION, NULL, (btaction)&bt_soldier::actionORight);
addChild("patrol", "follow_wpt", ACTION, NULL, (btaction)&bt_soldier::actionFollowWpt);
addChild("patrol", "next_wpt", ACTION, NULL, (btaction)&bt_soldier::actionNextWpt);

entity = new_entity;
}


int bt_soldier::actionReset()
{
printf("%s: handling idle\n",name.c_str());
return LEAVE;
}


int bt_soldier::actionChase()
{
printf("%s: handling pursuit\n",name.c_str());
return LEAVE;
}


int bt_soldier::actionCombat()
{
printf("%s: handling escape\n",name.c_str());
if (rand()%5) return LEAVE;
return STAY;
}

int bt_soldier::actionOLeft()
{
	printf("%s: handling shoot\n", name.c_str());
	return LEAVE;
}


int bt_soldier::actionORight()
{
	printf("%s: handling shootpistol\n", name.c_str());
	return LEAVE;
}

int bt_soldier::actionFollowWpt()
{
	printf("%s: handling shoot\n", name.c_str());
	return LEAVE;
}


int bt_soldier::actionNextWpt()
{
	printf("%s: handling shootpistol\n", name.c_str());
	return LEAVE;
}

bool bt_soldier::conditionReset() {

}
bool bt_soldier::conditionFight() {

}
bool bt_soldier::conditionChase() {

}
bool bt_soldier::conditionPatrol() {
	return true;
}



