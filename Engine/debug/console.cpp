#include "mcv_platform.h"
#include "console.h"

#include "components\entity_tags.h"
#include "handle\handle.h"
#include "components\comp_charactercontroller.h"
#include "components\comp_transform.h"
#include "components\entity.h"
#include "app_modules/logic_manager/logic_manager.h"

void CConsole::Command_TeleportXYZ(std::vector<char*> &args) {
	CEntity *e = tags_manager.getFirstHavingTag(getID("player"));
	if (e) {
		TCompCharacterController *cc = e->get<TCompCharacterController>();
		//double temp = strtod(args[0], NULL);
		float x = atof(args[0]);
		float y = atof(args[1]);
		float z = atof(args[2]);
		cc->teleport(PxVec3(x, y, z));
	}
}

void CConsole::Command_Teleport(std::vector<char*> &args) {
	if (args.size() == 0) {
		AddLog("[error] syntax error\n");
		AddLog("# format: teleport x y z OR teleport here (teleport camera position)\n");
		return;
	}

	if (Stricmp(args[0], "HERE") == 0) {

		CEntity *e_c = tags_manager.getFirstHavingTag(getID("camera_main"));
		CEntity *e_p = tags_manager.getFirstHavingTag(getID("player"));
		if (e_c && e_p) {

			TCompTransform *t = e_c->get<TCompTransform>();
			VEC3 pos = t->getPosition();

			pos += t->getFront()*2;

			float x = pos.x;
			float y = pos.y;
			float z = pos.z;

			TCompCharacterController *cc = e_p->get<TCompCharacterController>();
			cc->teleport(PxVec3(x, y, z));
		}

		return;
	}

	if (args.size() != 3) {
		AddLog("[error] syntax error\n");
		AddLog("# format: teleport x y z OR teleport here (teleport camera position)\n");
		return;
	}

	Command_TeleportXYZ(args);

}


void CConsole::Command_Lua(std::vector<char*> &args) {
	if (args.size() == 0) {
		AddLog("[error] syntax error\n");
		AddLog("# format: LUA need a list of lua commands\n");
		return;
	}
	for (auto arg : args) logic_manager->throwUserEvent(arg, "");
}