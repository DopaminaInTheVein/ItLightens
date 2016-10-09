#ifndef INC_WORKBENCH_H_
#define INC_WORKBENCH_H_

#include "components/components.h"

class workbench : public TCompBase {
public:
	void onCreate(const TMsgEntityCreated&);
};
#endif
