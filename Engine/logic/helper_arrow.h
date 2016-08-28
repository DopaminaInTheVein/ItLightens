#ifndef INC_HELPER_ARROW_H_
#define INC_HELPER_ARROW_H_

#include "components/components.h"
#include "geometry/geometry.h"
#include "logic/polarity.h"

struct LogicHelperArrow : public TCompBase {
	//Update info
	CHandle myEntity;
	TCompTransform * transform;
public:
	CHandle		target;
	void update(float elapsed);
	bool load(MKeyValue& atts);
	void init();
};
#endif
