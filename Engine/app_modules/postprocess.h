#ifndef INC_IPOSTPROCESS_H_
#define INC_IPOSTPROCESS_H_

#include "components/comp_base.h"

class IPostProcess : public TCompBase {
public:
	bool enabled;
	IPostProcess() : enabled(true) { }
	virtual void activate(){
		enabled = true;
	}

	virtual void deactivate() {
		enabled = false;
	}

	//virtual void renderDebug2D();
	virtual const char* getName() const = 0;
};

#endif
