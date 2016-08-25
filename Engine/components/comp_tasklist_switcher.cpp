#include "mcv_platform.h"
#include "comp_tasklist_switcher.h"
#include "comp_tasklist.h"
#include "utils/XMLParser.h"
#include "handle\handle.h"
#include "entity.h"
#include "entity_tags.h"

bool TasklistSwitch::load(MKeyValue & atts)
{
	task = atts.getInt("task", 0);
	return true;
}

bool TasklistSwitch::save(std::ofstream& ofs, MKeyValue& atts)
{
	atts.put("task", task);
	return true;
}

void TasklistSwitch::mUpdate(float dt)
{
	//nothing to do
}

void TasklistSwitch::onTriggerEnter(const TMsgTriggerIn & msg)
{
	CHandle h_in = msg.other;
	if (!h_in.hasTag("player")) { return; }
	CHandle tasklist_h = tags_manager.getFirstHavingTag(getID("tasklist"));
	CEntity * tasklist_e = tasklist_h;
	Tasklist* tasklist = tasklist_e->get<Tasklist>();
	tasklist->completeTask(task);
}

void TasklistSwitch::onCreate(const TMsgEntityCreated & msg)
{
}

void TasklistSwitch::init()
{
}