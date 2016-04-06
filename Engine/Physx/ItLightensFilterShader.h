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
		eANYONE = (1 << 2),
		eNPC = (1 << 3),
		eLIQUID = (1 << 4),
		eCRYSTAL = (1 << 5),
		eGUARD = (1 << 6),
		ePOSSEABLE = (1 << 7),
		eBOMB = (1 << 8),
		eOBJECT = (1 << 9),
		eSTATIC_OBJECT = (1 << 10),
		eALL_STATICS = (1 << 11),
		eALL_OBJECTS = (1 << 12),
	};

	//behaviour object
	enum descObjectBehaviour {
		eCOLLISION = (1 << 0),
		eCAN_TRIGGER = (1 << 2),
	};


	//-----------------------------------------------------------------------------------------------------
	//							      Filter shader
	//-----------------------------------------------------------------------------------------------------

	// customized filter shader
	// - Triggers dont collide
	// - only eCOLLISION objects can collide
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


		//pass collision only on objects with tag eCOLLISION
		if ((filterData1.word1 & eCOLLISION) && (filterData0.word1 & eCOLLISION))
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		else {
			return PxFilterFlag::eDEFAULT;
		}

		// generate contacts for all that were not filtered above
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		return PxFilterFlag::eDEFAULT;
	}
};

#endif