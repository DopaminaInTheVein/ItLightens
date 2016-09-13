#ifndef INC_COMP_TASKLIST_H_
#define	INC_COMP_TASKLIST_H_

#include "comp_base.h"
#include <vector>

class Tasklist : public TCompBase {
	int maxPoints;
	std::vector<VEC3> pointsPlaces;
	std::vector<int> pointsRooms;
	std::vector<bool> pointsVisiteds;
	CHandle end;
public:
	void init();
	bool save(std::ofstream& ofs, MKeyValue& atts);
	bool load(MKeyValue& atts);
	void completeTask(int i);
	void update(float dt);
	void renderInMenu();
};

#endif