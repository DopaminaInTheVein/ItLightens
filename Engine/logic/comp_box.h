#ifndef INC_BOX_COMPONENT_H_
#define	INC_BOX_COMPONENT_H_

#include "components/comp_base.h"
#include "handle/handle.h"

class TCompTransform;

struct TCompBox : public TCompBase {
	static VHandles all_boxes;
	bool moving = false;
	bool carePosition = true;
	bool removable = true;
	bool added = false;
	VEC3 size;
	int max_dot_index;
	float max_dot;
	enum eTypeBox {
		SMALL = 0,
		MEDIUM,
	} type_box;

	CHandle mParent;
	VEC3 originPoint;
	float dist_separation = 5.0f;
	float push_box_distance = 0.81f;

	~TCompBox();
	void init();
	void update(float elapsed);
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);

	void stuntNpcs();
	void ImTooFar();
	VEC3 GetLeavePoint() const;
	bool isRemovable();
	void UnGrab();
	void onUnLeaveBox(const TMsgLeaveBox& msg);
	bool getGrabPoints(TCompTransform * t_actor
		, VEC3& left
		, VEC3& right
		, VEC3& front_dir
		, VEC3& pos_grab
		, VEC3& normal_left
		, VEC3& normal_right
		, float offset_separation = 0.3f
		, bool recalc = true);
	bool getPushPoints(TCompTransform * t_actor
		, VEC3& left
		, VEC3& right
		, VEC3& front_dir
		, VEC3& pos_grab
		, VEC3& normal_left
		, VEC3& normal_right
		, float offset_separation = 0.3f
		, bool recalc = true);
};

#endif