#ifndef INC_COMPONENT_ROOM_H_
#define INC_COMPONENT_ROOM_H_

#include "utils/XMLParser.h"
#include "comp_base.h"
#include <sstream>
#include <set>

#define ROOM_LIMBO 99999999
#define ROOM_ALL -1
#define ROOM_MAX_SIZE	5
struct TRoom {
private:
	bool list[ROOM_MAX_SIZE] = { false };
	int special_room = ROOM_ALL;
public:
	TRoom() {}
	TRoom(int room);
	int getSingleRoom();
	std::set<int> getList();
	bool sameRoom(int r);

	void addRoom(int r);
	bool empty() {
		if (special_room != ROOM_ALL) return false;
		for (auto v : list) if (v) return false;
		return true;
	}
	std::string print();
};

struct TCompRoom : public TCompBase {
	static std::set<int> all_rooms;
	static inline bool SameRoom(TCompRoom* r, int r_id) {
		return (!r || r->sameRoom(r_id));
	}
	TRoom room;
public:
	//char rooms_raw[32];

	void init();

	bool load(MKeyValue& atts);

	bool save(std::ofstream& os, MKeyValue& atts);

	void setRoom(TRoom new_room);
	TRoom getRoom();
	int getSingleRoom();
	//bool sameRoom(const TRoom& r);
	//bool sameRoom(const CHandle& h);
	bool sameRoom(int r);
	bool sameRoomPlayer();
	void renderInMenu();
	void addRoom(int r);
};

#define ROOM_IS_IN TCompRoom::SameRoom

#endif
