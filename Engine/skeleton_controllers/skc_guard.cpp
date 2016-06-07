#include "mcv_platform.h"
#include "skc_guard.h"

void SkelControllerGuard::myUpdate()
{
	if (currentState == prevState) return;	

	// the guard only walks
	if (currentState == "run") currentState = "walk";
	// the attackgoback state combines 2 animations: walk backwards and attack
	else if (currentState == "attackgoback") {
		std::vector<std::string> anims;
		anims.push_back("attackgobackbody");
		anims.push_back("attackgoback");
		setLoop(anims);
		return;
	}

	setLoop(currentState);
	priority = false;
	//SkelController::myUpdate();
}