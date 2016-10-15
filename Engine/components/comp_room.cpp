#include "mcv_platform.h"
#include "comp_room.h"
#include "logic/sbb.h"

#include "player_controllers/player_controller_base.h"

#include "entity.h"

using namespace std;
set<int> TCompRoom::all_rooms = set<int>();

bool TCompRoom::load(MKeyValue& atts) {
	string to_parse = atts.getString("name", "-1");
	stringstream ss(to_parse);
	string number;
	//strcpy(rooms_raw, to_parse.c_str());
	while (getline(ss, number, '/')) {
		int value = stoi(number);
		if (value == ROOM_ALL) {
			room = TRoom();
			break;
		}
		room.addRoom(value);
		all_rooms.insert(value);
	}
	return true;
}

void TCompRoom::init()
{
	if (CHandle(this).getOwner().hasTag("player")) {
		SBB::postSala(room.getSingleRoom());
	}
}

bool TCompRoom::save(ofstream& os, MKeyValue& atts) {
	atts.put("name", room.print());
	return true;
}

void TCompRoom::renderInMenu() {
	stringstream ss;
	bool first = true;
	for (auto r : room.getList()) {
		if (!first) ss << "/";
		ss << r;
		first = false;
	}
	std::string txt = ss.str();
	ImGui::Text(txt.c_str());
}

void TCompRoom::setRoom(TRoom new_room) { room = new_room; }
TRoom TCompRoom::getRoom() {
	return room;
}
int TCompRoom::getSingleRoom() {
	return room.getSingleRoom();
}
bool TCompRoom::sameRoom(int r) {
	return room.sameRoom(r);
}
void TCompRoom::addRoom(int r) {
	room.addRoom(r);
}

//bool TCompRoom::sameRoom(const TRoom& r)
//{
//	return room.sameRoom(r);
//}
//bool TCompRoom::sameRoom(const CHandle& h)
//{
//	TCompRoom* other_room = h;
//	if (!other_room) other_room = GETH_COMP(h, TCompRoom);
//	return other_room->sameRoom(room);
//}

bool TCompRoom::sameRoomPlayer()
{
	int sala_player = SBB::readSala();
	bool res = room.sameRoom(sala_player);
	if (res && sala_player == 2) {
		//fast fix for room3
		CHandle hp = CPlayerBase::handle_player;
		if (hp.isValid()) {
			GET_COMP(t, hp, TCompTransform);
			GET_MY(tl, TCompTransform);
			if (t && tl) {
				if (t->getPosition().y > 10) {
					if (tl->getPosition().y < 12)
						res = false;
				}
				else {
					if (tl->getPosition().y > 12)
						res = false;
				}
			}
		}
	}
	return res;
}

bool TCompRoom::SameRoom(CHandle h, int r_id) {
	if (h.isValid()) {
		GET_COMP(room, h, TCompRoom);
		if (!room) room = GETH_COMP(h.getOwner(), TCompRoom);
		return room->sameRoom(r_id);
	}
	return true;
}

//TRoom functions
TRoom::TRoom(int room)
{
	addRoom(room);
}

void TRoom::addRoom(int r)
{
	if (r >= 0 && r < ROOM_MAX_SIZE)
		list[r] = true;
	else {
		*this = TRoom();
		special_room = r;
	}
}

set<int> TRoom::getList()
{
	std::set<int> res;
	if (special_room != ROOM_ALL) {
		res.insert(special_room);
	}
	else {
		for (int i = 0; i < ROOM_MAX_SIZE; i++) {
			if (list[i])
				res.insert(i);
		}
	}
	return res;
}

int TRoom::getSingleRoom()
{
	int res = special_room;
	if (res == ROOM_ALL) {
		int found = 0;
		for (int i = 0; i < ROOM_MAX_SIZE; i++) {
			if (list[i]) {
				res = i;
				found++;
			}
		}
		assert(found <= 1);
	}
	else {
		dbg("return special room\n");
	}
	return res;
}

string TRoom::print()
{
	if (empty()) {
		return "-1";
	}
	else {
		stringstream ss;
		bool first = true;
		for (int i = 0; i < ROOMS_SIZE; i++) {
			if (list[i]) {
				if (!first) ss << "/";
				ss << i;
				first = false;
			}
		}
		return ss.str();
	}
}

bool TRoom::sameRoom(int room)
{
	if (room == ROOM_ALL || empty()) return true;
	if (room >= ROOMS_SIZE) {
		return special_room == room;
	}
	return list[room];
}