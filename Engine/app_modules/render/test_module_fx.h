#ifndef TEST_FX
#define TEST_FX

#include "comp_basic_fx.h"

class TTestFx : public TCompBasicFX{
public:
	int id;
	void setId(int num) {
		id = num;
	}
	void ApplyFX() {
		//dbg("%d - executed test fx!\n", id);
	}

	const char* getName() const {
		return "test_fx";
	}
};

#endif