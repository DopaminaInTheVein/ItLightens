#ifndef INC_SLB_PUBLIC_FUNCTIONS_H_
#define	INC_SLB_PUBLIC_FUNCTIONS_H_

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "logic_manager.h"
#include "handle\handle.h"

// player handle control in LUA
class SLBPlayer
{
	CHandle player_handle;

public:

	// player functions
	void getPlayer();
	void setPlayerPosition(float x, float y, float z);
	float getPlayerX();
	float getPlayerY();
	float getPlayerZ();
};

// generic handle control in LUA
class SLBHandle
{
	CHandle real_handle;

public:

	// player functions
	void getHandleById(int id);
	void getHandleByNameTag(const char* name, const char* tag);
	void setPosition(float x, float y, float z);
	float getX();
	float getY();
	float getZ();
};

// public functions
class SLBPublicFunctions
{
public:

	// public functions
	void execCommand(const char* exec_code, float exec_time);
	void print(const char* to_print);

};

#endif
