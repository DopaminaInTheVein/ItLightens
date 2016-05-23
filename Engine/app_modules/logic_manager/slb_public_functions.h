#ifndef INC_SLB_PUBLIC_FUNCTIONS_H_
#define	INC_SLB_PUBLIC_FUNCTIONS_H_

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "logic_manager.h"
#include "app_modules\sound_manager\sound_manager.h"
#include "handle\handle.h"
#include "components/entity.h"

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
	void addEnergy(int energy_to_add);
	void refillEnergy();
};

// generic handle control in LUA
class SLBHandle
{
	CHandle real_handle;
	CHandle caller_handle;
	std::string handle_name;
	std::string handle_tag;

public:

	// generic handle functions
	void getHandleById(int id);
	void getHandleByNameTag(const char* name, const char* tag);
	void getHandleCaller();
	void destroy();
	void setPosition(float x, float y, float z);
	float getX();
	float getY();
	float getZ();
	void goToPoint(float x, float y, float z);
	void toggleGuardFormation();
	void setActionable(int);
	void activate();
	void setPolarity(int);
	void setLocked(int);
};

class SLBHandleGroup
{
	VHandles handle_group;

public:
	void getHandlesByTag(const char * tag);
	void setEnabled(int enabled);
};

// camera control in LUA
class SLBCamera
{
	CHandle camera_h;

public:

	// camera functions
	void getCamera();
	void setDistanceToTarget(float distance);
	void setSpeed(float speed);
	void setSpeedUnlocked(float speed);
	void setRotationSensibility(float sensibility);
	void setPositionOffset(float x_offset, float y_offset, float z_offset);
	void runCinematic(const char* name, float speed = 0.f);
};

// public functions
class SLBPublicFunctions
{
public:

	// public functions
	void execCommand(const char* exec_code, float exec_time);
	void print(const char* to_print);
	void setControlEnabled(int);
	void playSound(const char* sound_route);
	void playMusic(const char* music_route);
	void playVoice(const char* voice_route);
	void playAmbient(const char* ambient_route);
	void setSoundVolume(float volume);
	void setMusicVolume(float volume);
	void setVoicesVolume(float volume);
	void setAmbientVolume(float volume);
	void stopSoundChannel();
	void stopMusicChannel();
	void stopVoicesChannel();
	void stopAmbientChannel();
	void toggleIntroState();

	void test(const char* to_print);

};

#endif
