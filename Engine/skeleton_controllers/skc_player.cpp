#include "mcv_platform.h"
#include "skc_player.h"

#include "components/comp_charactercontroller.h"
#include "components/entity.h"

void SkelControllerPlayer::SetCharacterController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		cc = eMe->get<TCompCharacterController>();
		assert(cc || fatal("Player doesnt have character controller"));
	}
}

void SkelControllerPlayer::myUpdate()
{
	SetCharacterController();
	if (currentState == "walk") {
		VEC3 movement = cc->GetMovement();
		if ( !isNormal(movement) || movement.LengthSquared() < 0.1f) {
			currentState = "idle";
		}
	}

	if (currentState != prevState) {
		if (currentState == "jump") {
			setAction("jump");
		}
		else {
			setLoop(currentState);
		}
	}
}