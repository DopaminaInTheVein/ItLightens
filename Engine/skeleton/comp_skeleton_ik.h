#ifndef INC_COMP_SKELETON_IK_H_
#define INC_COMP_SKELETON_IK_H_

#include "ik.h"
#include "components/comp_base.h"

struct TCompSkeletonIK : public TCompBase {
	struct TBoneMod {
		VEC3  normal;
		int   bone_id_c;
		int   bone_id_b;
		int   bone_id_a;
		float dist_ab;
		float dist_bc;
		float time;
		float time_max;
		bool enabled;

		ClHandle h_solver;
		IK::bone_solver f_solver;
		IK::bone_solver f_ender;
	};

	std::vector<TBoneMod>  mods;
	void  solveBone(TBoneMod* bm);

	TCompSkeletonIK() {}
	void update(float elapsed);
	void onSetIKSolver(const TMsgSetIKSolver&);
	TBoneMod getBoneModInvariant(std::string);
};

#endif
