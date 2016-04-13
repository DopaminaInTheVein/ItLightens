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
		ePLAYER_CONTROLLED 	= (1 << 0),
		ePLAYER_BASE 		= (1 << 1),
		eLIQUID 			= (1 << 2),
		eCRYSTAL 			= (1 << 3),
		eGUARD 				= (1 << 4),
		ePOSSESSABLE 		= (1 << 5),
		eBOMB 				= (1 << 6),
		eOBJECT 			= (1 << 7),
		eSCENE 				= (1 << 8),
		eALL				= ~0,
	};

	//behaviour object
	enum descObjectBehaviour {
		eCOLLISION 		= (1 << 0),
		eCAN_TRIGGER 		= (1 << 1),
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
	static PxFilterFlags ItLightensFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}

		// generate default contact for no-triggers
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		//pass collision only on objects with tag eCOLLISION
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1) && (filterData1.word2 & eCOLLISION) && (filterData0.word2 & eCOLLISION))
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		else {
			return PxFilterFlag::eDEFAULT;
		}

		
		

		return PxFilterFlag::eDEFAULT;
	}
};

#endif