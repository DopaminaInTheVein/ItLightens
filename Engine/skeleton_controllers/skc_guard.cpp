#include "mcv_platform.h"
#include "skc_guard.h"

void SkelControllerGuard::myUpdate()
{
	if (currentState == "run") currentState = "walk";
	//dbg(" %s --> %s\n", prevState.c_str(), currentState.c_str());
	if (prevState == "walk" && currentState == "idle") {
		dbg("Now idle!\n");
	}
	if (prevState == "idle" && currentState == "walk") {
		dbg("Now walk!\n");
	}
	SkelController::myUpdate();
}