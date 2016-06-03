#include "mcv_platform.h"
#include "skc_guard.h"

void SkelControllerGuard::myUpdate()
{
	// the guard only walks
	if (currentState == "run") currentState = "walk";
	// the attackgoback state combines 2 animations: walk backwards and attack
	else if (currentState == "attackgoback") {
		std::vector<std::string> anims;
		anims.push_back("attackgoback");
		anims.push_back("attack");

		setLoop(anims);
	}

	SkelController::myUpdate();
}