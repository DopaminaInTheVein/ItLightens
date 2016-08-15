#ifndef INC_HELPER_ARROW_H_
#define INC_HELPER_ARROW_H_

#include "components/components.h"
#include "geometry/geometry.h"
#include "logic/polarity.h"

struct LogicHelperArrow : public TCompBase {
	//Update info
	ClHandle myEntity;
	TCompTransform * transform;
public:
	ClHandle		target;
	void update(float elapsed);
	bool load(MKeyValue& atts);
	void onSetTarget(const TMsgSetTarget& tasr);
	void init();
};
#endif
