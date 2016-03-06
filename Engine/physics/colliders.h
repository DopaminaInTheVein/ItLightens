#ifndef INC_COLLIDERS_H_
#define INC_COLLIDERS_H_

#include "components\comp_base.h"
#include "components\comp_msgs.h"

struct sphereCollider : public TCompBase {
	float	r;
	float	offset;		//distance to move center
	char	tag[64];

	VEC3 getCenter() const;
	float getRadius() const { return r; };
	std::string getTag() const { return std::string(tag); }
	bool load(MKeyValue& atts);
	void onCreate(const TMsgEntityCreated&);
};

struct boxCollider : public TCompBase {
	VEC3	relative_p1;
	VEC3	relative_p2;
	char	tag[64];
	int		types;

	VEC3 getPMAX() const;
	VEC3 getPMIN() const;
	bool load(MKeyValue& atts);
	std::string getTag() const { return std::string(tag); }
	void onCreate(const TMsgEntityCreated&);

	// RayCast (sólo el más cercano)
	void rayCast();
	void update(float dt);
};

#endif