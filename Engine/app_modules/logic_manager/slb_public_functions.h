#ifndef INC_SLB_PUBLIC_FUNCTIONS_H_
#define	INC_SLB_PUBLIC_FUNCTIONS_H_

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "logic_manager.h"
#include "handle\handle.h"

class SLBHandle
{
	CHandle real_handle;

public:

	void getPlayer();
	// generic functions
	void setPosition(float x, float y, float z);
	float getX();
	float getY();
	float getZ();
};

class SLBPublicFunctions
{
public:

	// public functions
	void execCommand(const char* exec_code, float exec_time);
	void print(const char* to_print);

};

#endif
