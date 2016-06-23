#ifndef INC_SKELETON_IK_H_
#define INC_SKELETON_IK_H_

#include "geometry/geometry.h"
#include "handle/handle.h"

namespace IK {
	struct InfoSolver {
		CHandle handle;
		VEC3 bone_pos;
	};
	struct ResultSolver {
		VEC3 offset_pos;
	};

	//template<typename TObj>
	//struct IKSolver {
	//	TObj * object;
	//	typedef void(TObj::*bone_solver2)(const InfoSolver&, ResultSolver&);
	//	bone_solver2 function;
	//	IKSolver(TObj* c, bone_solver2 bs) : caller(c), solver(bs) {}
	//	void call(const InfoSolver& info, ResultSolver& result) {
	//		object->function(info, result);
	//	}
	//};

	typedef void(*(bone_solver))(const InfoSolver&, ResultSolver&);
}
//IK::ResultSolver heilTest(const IK::InfoSolver&);
#define IK_DECL_SOLVER(name) void name(const IK::InfoSolver&, IK::ResultSolver&)
#define IK_IMPL_SOLVER(name, info, result) void name(const IK::InfoSolver& info, IK::ResultSolver& result)

#endif
