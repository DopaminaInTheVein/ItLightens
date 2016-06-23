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

		//TIKHandle ik;
		CHandle h_solver;
		IK::bone_solver f_solver;
	};

	float     amount;
	std::vector<TBoneMod>  mods;
	void  solveBone(TBoneMod* bm);
	//IK_DECL_SOLVER(heilTest);

	TCompSkeletonIK() : amount(1.0f) {}
	bool load(MKeyValue &atts);
	void update(float elapsed);
	void onCreate(const TMsgEntityCreated&);
	TBoneMod getBoneModInvariant(std::string);
};

#endif
