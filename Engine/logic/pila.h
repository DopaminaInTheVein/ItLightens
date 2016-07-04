#ifndef INC_LOGIC_PILA_H_
#define	INC_LOGIC_PILA_H_

#include "components/comp_base.h"
#include "handle/handle.h"

#include "components/comp_msgs.h"

class TCompTransform;

struct TCompPila : public TCompBase {
	static VHandles all_pilas;

	~TCompPila();
	void onCreate(const TMsgEntityCreated& msg);

	void update(float elapsed);
	bool load(MKeyValue& atts);
};

#endif