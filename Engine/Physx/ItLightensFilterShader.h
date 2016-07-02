#ifndef INC_PHYSX_FILTER_SHADER_H
#define INC_PHYSX_FILTER_SHADER_H

#include "PxPhysicsAPI.h"
//#include "physx_manager.h"

using namespace physx;

class ItLightensFilter {
public:

	//-----------------------------------------------------------------------------------------------------
	//							      FLAGS
	//-----------------------------------------------------------------------------------------------------

	//type object
	enum descObjectFlags {
		ePLAYER_CONTROLLED = (1 << 0),
		ePLAYER_BASE = (1 << 1),
		eLIQUID = (1 << 2),
		eCRYSTAL = (1 << 3),
		eGUARD = (1 << 4),
		ePOSSESSABLE = (1 << 5),
		eBOMB = (1 << 6),
		eOBJECT = (1 << 7),
		eSCENE = (1 << 8),
		eBIG_OBJECT = (1 << 9),
		eFRAGMENT = (1 << 10),
		eTHROW = (1 << 11),
		ePLATFORM = (1 << 12),
		eALL = ~0,
	};

	//behaviour object
	enum descObjectBehaviour {
		eCOLLISION = (1 << 0),
		eCAN_TRIGGER = (1 << 1),
		eUSER_CALLBACK = (1 << 2),
		eIGNORE_PLAYER = (1 << 3),
	};

	//-----------------------------------------------------------------------------------------------------
	//							      Filter shader
	//-----------------------------------------------------------------------------------------------------
	// customized filter shader
	// - Triggers dont collide
	// - only eCOLLISION objects can collide
	// - mask0: identity of object
	// - mask1: collisions against who can collider
	// - mask2: collisions activated
#define ILFS_FIRST filterData0
#define ILFS_SECOND filterData1
#define ILFS_SOME_HAS(flag, numWord) ILFS_FIRST.word##numWord & flag || ILFS_SECOND.word##numWord & flag
#define ILFS_BOTH_HAS(flag, numWord) ILFS_FIRST.word##numWord & flag && ILFS_SECOND.word##numWord & flag
#define ILFS_PAIR_OF(flag1, flag2) (ILFS_FIRST.word0 & flag1 && ILFS_SECOND.word0 & flag2) || (ILFS_FIRST.word0 & flag2 && ILFS_SECOND.word0 & flag1)

	static PxFilterFlags ItLightensFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
#ifndef NDEBUG
		if (filterData0.word2 & eUSER_CALLBACK || filterData1.word2 & eUSER_CALLBACK) {
			dbg("Colision caja callback\n");
		}
#endif

		PxFilterFlags result = PxFilterFlag::eKILL;
		PxU32 me, other;

		// TRIGGERS
		// -------------------------------------------------------------------------------
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}
		// -------------------------------------------------------------------------------

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		// FRAGMENT
		//--------------------------------------------------------------------------------
		if (ILFS_SOME_HAS(eFRAGMENT, 0)) {
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eCCD_LINEAR;
			pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
			result = PxFilterFlag::eDEFAULT;
		}
		//--------------------------------------------------------------------------------

		// PLAYER vs PLATFORM
		//--------------------------------------------------------------------------------
		else if (ILFS_PAIR_OF(ePLAYER_BASE, ePLATFORM, 0)) {
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags &= ~PxPairFlag::eSOLVE_CONTACT;
			result = PxFilterFlag::eCALLBACK;
		}
		//--------------------------------------------------------------------------------

		// GUARD vs PLATFORM (?)
		//--------------------------------------------------------------------------------
		else if (ILFS_PAIR_OF(eGUARD, ePLATFORM)) {
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags &= ~PxPairFlag::eSOLVE_CONTACT;
			result = PxFilterFlag::eCALLBACK;
		}
		//--------------------------------------------------------------------------------

		//BULLETS
		//--------------------------------------------------------------------------------
		else if ((me = filterData0.word0) & eTHROW || (me = filterData1.word0) & eTHROW) {
			other = (me == filterData0.word0) ? filterData1.word0 : filterData1.word0;
			if (other & eTHROW || other & ePLAYER_CONTROLLED) {
				pairFlags &= ~PxPairFlag::eCONTACT_DEFAULT;
				pairFlags &= ~PxPairFlag::eNOTIFY_TOUCH_FOUND;
				pairFlags &= ~PxPairFlag::eSOLVE_CONTACT;
				result = PxFilterFlag::eKILL;
			}
			else {
				pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
				pairFlags |= PxPairFlag::eSOLVE_CONTACT;
				result = PxFilterFlag::eCALLBACK;
			}
		}
		//--------------------------------------------------------------------------------

		//REGULAR COLLISION
		//--------------------------------------------------------------------------------
		else if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1)) {
			result = PxFilterFlag::eDEFAULT;
			if (ILFS_BOTH_HAS(eCOLLISION, 2)) {
				pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			}
			if (ILFS_SOME_HAS(eTHROW, 0)) {
				pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
				pairFlags &= ~PxPairFlag::eSOLVE_CONTACT;
				result = PxFilterFlag::eCALLBACK;
			}
			if ((me = filterData0.word0) & ePLAYER_CONTROLLED || (me = filterData1.word0) & ePLAYER_CONTROLLED) {
				other = (me == filterData0.word0) ? filterData1.word2 : filterData0.word2;
				if (other & eIGNORE_PLAYER) {
					pairFlags &= ~PxPairFlag::eSOLVE_CONTACT;
					return PxFilterFlag::eKILL;
				}
			}
			//Check if has user callback
			if (ILFS_SOME_HAS(eUSER_CALLBACK, 2)) {
				pairFlags &= ~PxPairFlag::eSOLVE_CONTACT;
				pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
				pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
				pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
				result = PxFilterFlag::eCALLBACK;
			}
		}
		//--------------------------------------------------------------------------------

		return result;
	}
};

#define PHYS_TAG ItLightensFilter::descObjectFlags
#define PHYS_BEHAVIOUR ItLightensFilter::descObjectBehaviour

#endif