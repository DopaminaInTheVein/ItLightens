#include "mcv_platform.h"
#include "comp_tasklist.h"
#include "handle/handle.h"
#include "entity.h"
#include "comp_transform.h"
#include "comp_room.h"
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
		while (pointsVisiteds[aux]) {
			aux++;
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
	TCompTransform * t = e->get<TCompTransform>();
	t->setPosition(pointsPlaces[0]);
	SBB::postInt("current_tasklist", 0);
}

void Tasklist::update(float dt) {
	CHandle h = CHandle(this).getOwner();
	CEntity * e = h;
	TCompRoom * room = e->get<TCompRoom>();
	std::vector<int> rooms(1);
	CHandle player = tags_manager.getFirstHavingTag(getID("player"));
	if (controller->IsSenseButtonPressed() && player.hasTag("raijin")) {
		rooms[0] = pointsRooms[SBB::readInt("current_tasklist")];
	}
	else {
		rooms[0] = 99999999;
	}
	room->setName(rooms);
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
}