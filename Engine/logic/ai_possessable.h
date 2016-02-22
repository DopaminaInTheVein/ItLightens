#ifndef INC_AI_POSSESSABLE_H_
#define INC_AI_POSSESSABLE_H_

#include "aicontroller.h"
#include "components/comp_base.h"
#include "components/comp_transform.h"
#include "components/entity.h"
#include "handle/handle.h"
#include "handle/object_manager.h"
#include "handle/handle_manager.h"
#include <vector>

class ai_possessable
{
public:
	static enum BOTS {
		SCIENTIST, MOLE, SPEEDY
	};
	void configPossession(BOTS bot, float energySpeed);
	void onPossessionStart(CHandle);
	void onPossessionEnd(CHandle);

private:
	float maxEnergy;
	float energyRemain;
	float energySpeed;
	BOTS botType;
};

#define DECLARE_POSSESSION(bot, energySpeed)	\
void ai_scientific::onPossessionStart(const TMsgPossession& msg) { \
	ai_possessable::onPossessionStart(CHandle(this)); \
} \
void ai_scientific::onPossessionEnd(const TMsgPossession& msg) { \
	ai_possessable::onPossessionEnd(CHandle(this)); \
}

#endif