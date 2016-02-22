#include "mcv_platform.h"
#include <windows.h>
#include "ai_possessable.h"
#include "components/entity_tags.h"
#include "utils/XMLParser.h"

/*void ai_possessable::configPossession(BOTS bot, float newEnergySpeed) {
	energySpeed = newEnergySpeed;
	botType = bot;
}*/

void ai_possessable::onPossessionStart(CHandle myHandle) {
	CEntity* me = myHandle;
	dbg("Possessed!");
}

void ai_possessable::onPossessionEnd(CHandle myHandle) {
	CEntity* me = myHandle;
	dbg("Possession Ends");
}