#ifndef INC_SKELETON_MANAGER_H_
#define INC_SKELETON_MANAGER_H_

#include "cal3d/cal3d.h"

//-------------- Utils --------------------
CalVector Engine2Cal(VEC3 v);
CalQuaternion Engine2Cal(CQuaternion q);
VEC3 Cal2Engine(CalVector v);
CQuaternion Cal2Engine(CalQuaternion q);
CalQuaternion getRotationFromAToB(CalVector a, CalVector b, float unit_amount, float max_angle = deg2rad(180.f));
//------------------------------------------------------------------------------

// Mas otras cosas que cal3d no tiene
class CCoreModel : public CalCoreModel {
public:
	CCoreModel();

	struct TBoneCorrector {
		int         bone_id;
		CalVector   local_dir;
		float       local_amount;
		bool        render;
		TBoneCorrector() : bone_id(-1), local_dir(1, 0, 0) {}
		TBoneCorrector(int abone_id, CalVector alocal_dir)
			: bone_id(abone_id)
			, local_dir(alocal_dir)
		{}
		void apply(CalModel* model, CalVector target, float amount, float max_angle = deg2rad(180.f), bool keep_vertical = false);
	};

	typedef std::vector <TBoneCorrector> VBoneCorrections;
	VBoneCorrections bone_corrections;
};

#define SK_RHAND "right_hand"
#define SK_LHAND "left_hand"

#endif
