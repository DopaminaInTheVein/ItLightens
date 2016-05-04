#include "mcv_platform.h"
#include "skc_player.h"

#include "components/comp_charactercontroller.h"
#include "player_controllers/player_controller.h"
#include "components/entity.h"

void SkelControllerPlayer::SetCharacterController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		cc = eMe->get<TCompCharacterController>();
		assert(cc || fatal("Player doesnt have character controller"));
	}
}

void SkelControllerPlayer::SetPlayerController() {
	if (owner.isValid()) {
		CEntity* eMe = owner;
		pc = eMe->get<player_controller>();
		assert(pc || fatal("Player doesnt have player controller"));
	}
}

void SkelControllerPlayer::myUpdate()
{
	SetCharacterController();
	if (currentState == "walk") {
		//TODO: read moving param!
		VEC3 speed = cc->GetSpeed();
		VEC3 lastSpeed = cc->GetLastSpeed();
		speed.y = 0; // No tenemos en cuenta la velocidad vertical!
		lastSpeed.y = 0; // No tenemos en cuenta la velocidad vertical!
		dbg("Player Speed, Last: %f, %f\n", speed.LengthSquared(), lastSpeed.LengthSquared());

		if ( !isNormal(speed) || speed.LengthSquared() <= 0.0001f) {
			currentState = "idle";
		}
	}

	if (currentState != prevState) {
		if (currentState == "jump") {
			setAction("jump", "jumpidle");
		}
		else {
			setLoop(currentState);
		}
	}
}