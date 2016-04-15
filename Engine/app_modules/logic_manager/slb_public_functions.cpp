#include "mcv_platform.h"
#include "slb_public_functions.h"

#include "handle\handle.h"
#include "components\entity.h"
#include "components\comp_name.h"

// public functions
void SLBPublicFunctions::execCommand(const char* exec_code, float exec_time) {
	// create the new command
	command new_command;
	new_command.code = exec_code;
	new_command.execution_time = exec_time;
	// add the new command to the queue
	logic_manager->getCommandQueue()->push_back(new_command);
}

void SLBPublicFunctions::print(const char* to_print) {
	Debug->LogWithTag("LUA","%s\n",to_print);
}



//test
void SLBPublicFunctions::test(const char* to_print) {

	//CHandle h = CHandle();
	//h.fromUnsigned(h_num);

	//if (h.isValid()) {
	//	CEntity *e = h;
	//	TCompName *name = e->get<TCompName>();
	Debug->LogWithTag("LUA", "%s\n", to_print);
	//}
}

