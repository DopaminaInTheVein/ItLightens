#include "mcv_platform.h"
#include "comp_generator.h"
#include "utils/XMLParser.h"
#include "handle\handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_render_static_mesh.h"
#include "entity_tags.h"

#include "utils/utils.h"

using namespace std;

VHandles TCompGenerator::all_generators;

bool TCompGenerator::load(MKeyValue & atts)
{
	rad = atts.getFloat("rad", 1);
	return true;
}

void TCompGenerator::mUpdate(float dt)
{
	//Ya esta activo
	if (____TIMER__END_(timeReuse)) return;

	//Esta inactivo, comprobamos si se reactiva
	____TIMER__UPDATE_(timeReuse);
	if (____TIMER__END_(timeReuse)) {
		setUsable(true);
	}
}

void TCompGenerator::onTriggerEnter(const TMsgTriggerIn & msg)
{
	CHandle h_in = msg.other;
	if (h_in.hasTag("raijin")) {
		CanRec(true);
	}
}

void TCompGenerator::onTriggerExit(const TMsgTriggerOut & msg)
{
	CHandle h_in = msg.other;
	if (h_in.hasTag("raijin")) {
		CanRec(false);
	}
}

void TCompGenerator::onCreate(const TMsgEntityCreated & msg)
{
	CHandle me_h = CHandle(this).getOwner();

	CEntity *me_e = me_h;
	TCompTransform *t = me_e->get<TCompTransform>();

	org = t->getPosition();

	CApp &app = CApp::get();
	std::string file_ini = app.file_initAttr_json;
	map<string, float> fields_base = readIniAtrData(file_ini, "generator");
	assignValueToVar(_timeReuse, fields_base);
	assignValueToVar(life_recover, fields_base);
	____TIMER_REDEFINE_(timeReuse, _timeReuse);
	____TIMER__SET_ZERO_(timeReuse);
	mesh = TCompRenderStaticMesh::last_loaded_static_mesh;
	setUsable(true);

	all_generators.push_back(me_h);
}

void TCompGenerator::init()
{
	player = tags_manager.getFirstHavingTag(getID("raijin"));
}

void TCompGenerator::CanRec(bool new_range)
{
	TMsgCanRec msg;
	CEntity *p_e = player;
	TCompTransform *t_p = p_e->get<TCompTransform>();
	VEC3 player_position = t_p->getPosition();

	msg.generator = new_range ? CHandle(this).getOwner() : CHandle();
	CEntity *player_e = player;
	player_e->sendMsg(msg);
}

void TCompGenerator::setUsable(bool usable)
{
	TMsgSetTag msg;
	msg.add = usable;
	msg.tag = "interactive";
	mesh.sendMsg(msg);
	if (!usable) {
		____TIMER_RESET_(timeReuse);
	}
}

bool TCompGenerator::isUsable()
{
	return ____TIMER__END_(timeReuse);
}

float TCompGenerator::use()
{
	if (!isUsable()) return 0;
	setUsable(false);
	return life_recover;
}

TCompGenerator::~TCompGenerator()
{
	removeFromVector(all_generators, CHandle(this).getOwner());
}