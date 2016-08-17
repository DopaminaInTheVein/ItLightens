#ifndef INC_COMPONENT_SKELETON_H_
#define INC_COMPONENT_SKELETON_H_

#include "components/comp_base.h"
#include "cal3d/cal3d.h"
#include "skeleton.h"

class CalModel;
class CSkeleton;
class TCompAbsAABB;
class TCompTransform;

struct TCompSkeleton : public TCompBase {
	const CSkeleton* resource_skeleton;
	CalModel*  model;
	CHandle culling;
	TCompAbsAABB * aabb;
	TCompTransform * tmx;

	//Render debug
	static float dt_frame;
	static int total_skeletons;
	static int updated_skeletons;

	std::string res_name;
	std::vector<int> prevCycleIds;
	bool load(MKeyValue& atts);
	bool save(std::ofstream& os, MKeyValue& atts);
	bool getUpdateInfo();
	void update(float dt);
	void render() const;
	void renderInMenu();
	static void renderUICulling();
	void uploadBonesToCteShader() const;
	void updateEndAction();
	void clearPrevAnims(bool instant);
	std::string getKeyBoneName(std::string);
	int getKeyBoneId(std::string);

	//Messages
	void onSetAnim(const TMsgSetAnim&);
};

#endif
