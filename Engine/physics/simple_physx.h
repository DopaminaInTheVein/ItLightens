#ifndef INC_SIMPLE_PHYSX_H_
#define	INC_SIMPLE_PHYSX_H_

#include <map>

class CHandle;
using namespace std;

class CSimplePhysx {

	map<string, vector<string>> relations;

	bool isColliding(CHandle own, CHandle other);


	//real collision detection functions
	bool SphereVsSphere(float r1, VEC3 c1,  float r2, VEC3 c2);
	bool SphereVsBox(float r, VEC3 c, VEC3 pos_min, VEC3 pos_max);

public:
	CSimplePhysx(){

		//relations of objects by tags for collisions

		//player physx relations
		relations["player"] = { "npc", "wall", "platform", "objects" };

		//enemy
		relations["npc"] = {"platform", "objects" };

	}

	bool isMovementValid(CHandle h, std::string tag);

	void test();		//TO REMOVE
	

};

#endif