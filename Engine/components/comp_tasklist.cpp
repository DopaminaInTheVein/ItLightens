#include "mcv_platform.h"
#include "comp_tasklist.h"
#include "handle/handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_room.h"
#include "comp_render_static_mesh.h"
#include "logic/sbb.h"

// Loading the wpts
#define WPT_ATR_NAME(nameVariable, nameSufix, index) \
char nameVariable[10]; sprintf(nameVariable, "%s%d", nameSufix,index);

bool Tasklist::load(MKeyValue& atts) {
	maxPoints = atts.getInt("points_size", 0);
	pointsPlaces.resize(maxPoints);
	pointsRooms.resize(maxPoints);
	pointsVisiteds.resize(maxPoints);
	for (int i = 0; i < maxPoints; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		pointsPlaces[i] = atts.getPoint(atrPos);
		WPT_ATR_NAME(atrRoom, "room", i);
		pointsRooms[i] = atts.getInt(atrRoom, -1);
		WPT_ATR_NAME(atrVisited, "visited", i);
		pointsVisiteds[i] = atts.getBool(atrVisited, false);
	}
	return true;
}

void Tasklist::completeTask(int i) {
	if (i < maxPoints) {
		pointsVisiteds[i] = true;
	}
	if (i == SBB::readInt("current_tasklist")) {
		int aux = i + 1;
		while (aux < maxPoints - 1 && pointsVisiteds[aux]) {
			aux++;
		}
		if (aux >= maxPoints) {
			aux = maxPoints - 1;
		}
		SBB::postInt("current_tasklist", aux);
		CHandle h = CHandle(this).getOwner();
		CEntity * e = h;
		TCompTransform * t = e->get<TCompTransform>();
		t->setPosition(pointsPlaces[aux]);
	}
}

void Tasklist::init() {
	CHandle h = CHandle(this).getOwner();
	CEntity * e = h;
	TCompRoom * room = e->get<TCompRoom>();

	std::vector<int> rooms(1);
	rooms[0] = 99999999;
	room->setName(rooms);

	TCompTransform * t = e->get<TCompTransform>();
	t->setPosition(pointsPlaces[0]);
	SBB::postInt("current_tasklist", 0);

	auto hm = CHandleManager::getByName("entity");
	CHandle new_hp = hm->createHandle();
	CEntity* entity = new_hp;

	auto hm1 = CHandleManager::getByName("name");
	CHandle new_hn = hm1->createHandle();
	MKeyValue atts1;
	atts1.put("name", "tasklistend");
	new_hn.load(atts1);
	entity->add(new_hn);

	auto hm3 = CHandleManager::getByName("tags");
	CHandle new_hl = hm3->createHandle();
	MKeyValue atts3;
	atts3["tags"] = "tasklistend";
	new_hl.load(atts3);
	entity->add(new_hl);

	auto hm4 = CHandleManager::getByName("transform");
	CHandle new_ht = hm4->createHandle();
	MKeyValue atts4;
	atts4.put("pos", pointsPlaces[maxPoints - 1]);
	atts4.put("quat", t->getRotation());
	atts4.put("scale", t->getScale());
	new_ht.load(atts4);
	entity->add(new_ht);

	TCompRenderStaticMesh * rsm = e->get<TCompRenderStaticMesh>();

	auto hm5 = CHandleManager::getByName("render_static_mesh");
	CHandle new_r = hm5->createHandle();
	MKeyValue atts5;
	atts5.put("name", rsm->res_name);
	new_r.load(atts5);
	entity->add(new_r);

	auto hm6 = CHandleManager::getByName("room");
	CHandle new_roo = hm6->createHandle();
	MKeyValue atts6;
	atts6["name"] = "-1";
	new_roo.load(atts6);
	entity->add(new_roo);

	end = new_hp;

	TMsgEntityCreated msg;
	CEntity * mstmesh = end;
	mstmesh->sendMsg(msg);
}

void Tasklist::update(float dt) {
	CHandle h = CHandle(this).getOwner();
	CEntity * e = h;
	TCompRoom * room = e->get<TCompRoom>();

	CEntity * e2 = end;
	TCompRoom * room2 = e2->get<TCompRoom>();

	std::vector<int> rooms(1);
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (controller->IsSenseButtonPressed() && player.hasTag("raijin")) {
		rooms[0] = pointsRooms[SBB::readInt("current_tasklist")];
	}
	else {
		rooms[0] = 99999999;
	}
	room->setName(rooms);
	room2->setName(rooms);
}

bool Tasklist::save(std::ofstream& ofs, MKeyValue& atts) {
	atts.put("points_size", maxPoints);
	for (int i = 0; i < maxPoints; i++) {
		WPT_ATR_NAME(atrPos, "pos", i);
		atts.put(atrPos, pointsPlaces[i]);
		WPT_ATR_NAME(atrRoom, "room", i);
		atts.put(atrRoom, pointsRooms[i]);
		WPT_ATR_NAME(atrVisited, "visited", i);
		atts.put(atrVisited, pointsVisiteds[i]);
	}
	return true;
}

void Tasklist::renderInMenu() {
	for (int i = 0; i < maxPoints; i++) {
		ImGui::Text("Point task %d is %s", i, pointsVisiteds[i] ? "done" : "todo");
	}
}