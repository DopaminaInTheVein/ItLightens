#include "mcv_platform.h"
#include "comp_snoozer.h"

#include "components/entity.h"
#include "components/entity_parser.h"

void TCompSnoozer::onPreload(const TMsgPreload& msg) {
	components = new CPrefabCompiler;
	*components = *(msg.comps);
	CEntity* e = CHandle(this).getOwner();
	e->setReload(false);
}

void TCompSnoozer::onAwake(const TMsgAwake& msg) {
	CHandle parent = CHandle(this).getOwner();
	CEntityParser ep = CEntityParser(parent);
	components->execute(&ep);
	TMsgEntityCreated msgCreated;
	((CEntity*)parent)->sendMsg(msgCreated);
	((CEntity*)parent)->setReload(true);
	CHandle(this).destroy();
}

void TCompSnoozer::renderInMenu() {
	for (auto c : components->calls) {
		if (c.is_start)	ImGui::Text(c.elem.c_str());
	}
}