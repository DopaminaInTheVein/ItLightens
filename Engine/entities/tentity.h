#ifndef _TENTITY
#define _TENTITY

#include "../geometry/transform.h"

class TEntity
{
public:
	CTransform   transform;
	std::string  name;
	//IResource*   mesh;
	//const CMesh* mesh;

	TEntity() {
		transform = CTransform();
		name = "Patrullero";
	}
};

#endif