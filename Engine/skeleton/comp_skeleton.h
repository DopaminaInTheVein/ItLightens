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
	bool first_update = false;

	//Render debug
	static float dt_frame;
	static int total_skeletons;
	static int updated_skeletons;

	std::string res_name;
	std::string anim_default;
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
	float getLFootOffset() {
		auto non_const_skel = const_cast<CSkeleton*>(resource_skeleton);
		return non_const_skel->getLFootOffset();
	}
	float getRFootOffset() {
		auto non_const_skel = const_cast<CSkeleton*>(resource_skeleton);
		return non_const_skel->getRFootOffset();
	}

	VEC3 getBonePos(std::string key_bone) {
		int bone_id = getKeyBoneId(key_bone);
		auto bone = model->getSkeleton()->getBone(bone_id);
		return Cal2Engine(bone->getTranslationAbsolute());
	}

	//Messages
	void onSetAnim(const TMsgSetAnim&);
};

#endif
