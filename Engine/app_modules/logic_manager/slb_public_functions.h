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
class SLBPosition
{
	VEC3 pos;

public:
	float X();
	float Y();
	float Z();
	void setXYZ(float, float, float);
	VEC3 get();
	void set(VEC3);
};

class SLBPlayer
{
	CHandle player_handle;

public:

	// player functions
	void getPlayer();
	void getRaijin();
	void setPlayerPosition(float x, float y, float z);
	void teleport(const char * point_name);
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

	//Aux
	void _setAnim(const char* name, bool loop);
public:
	// Gets
	CHandle getHandle();

	// generic handle functions
	void getPlayer();
	void getRaijin();
	void getHandleById(int id);
	void getHandleByNameTag(const char* name, const char* tag);
	void getHandleCaller();
	void destroy();
	const char* getName();
	bool isValid();
	SLBPosition getPos();
	void setPos(SLBPosition);
	void setPosition(float x, float y, float z);
	float getX();
	float getY();
	float getZ();
	void goToPoint(float x, float y, float z);
	void goAndLookAs(SLBHandle target, std::string code_arrived);
	void followTracker(SLBHandle target, float speed);
	void toggleGuardFormation();
	void toggleScientistBusy();
	void setActionable(int);
	int  activate();
	void setPolarity(int);
	void setLocked(int);
	bool hasPila();
	bool hasPilaCharged();
	void setCharged(bool);
	bool isCharged();
	void setAnim(const char* name);
	void setAnimLoop(const char* name);
	bool isPatrolling();
	bool isComeBack();
};

class SLBHandleGroup
{
	VHandles handle_group;

public:
	void getHandlesByTag(const char * tag);
	void awake();
	void removePhysics();
	void destroy();
};

// camera control in LUA
class SLBCamera
{
	CHandle camera_h;

public:

	// camera functions
	void getCamera();
	bool checkCamera();
	void setDistanceToTarget(float distance);
	void setSpeed(float speed);
	void setSpeedUnlocked(float speed);
	void setRotationSensibility(float sensibility);
	void setPositionOffset(float x_offset, float y_offset, float z_offset);
	void runCinematic(const char* name, float speed = 0.f);
	void fadeIn(float speed = 0.f);
	void fadeOut(float speed = 0.f);
	void resetCamera();
};

// public functions
class SLBData
{
	std::string file_name;
	std::map<std::string, float> data;
public:
	SLBData();
	float getFloat(const char* key);
	bool getBool(const char* key);
	void putFloat(const char* key, float value);
	void putBool(const char* key, bool value);
	// public functions
	void write();
};

// public functions
class SLBPublicFunctions
{
public:

	// public functions
	void execCommand(const char* exec_code, float exec_time);
	void completeTasklist(int i);
	void print(const char* to_print);
	void setControlEnabled(int);
	void setPlayerEnabled(int);
	void setCameraEnabled(int);
	void setOnlySense(int);
	void playSound(const char* sound_route);
	void play3dSound(const char* sound_route, float pl_x, float pl_y, float pl_z, float s_x, float s_y, float s_z);
	void playMusic(const char* music_route);
	void playLoopingMusic(const char* music_route);
	void playVoice(const char* voice_route);
	void playAmbient(const char* ambient_route);
	void playVideo(const char* video_route);
	void playVideoAndDo(const char* video_route, const char* lua_code);
	void playerRoom(int newRoom);
	void playerTalks(const char* text, const char* iconName, const char* iconText);
	void playerTalksWithColor(const char* text, const char* iconName, const char* iconText, const char* background, const char* textColor);
	void characterGlobe(float distance, float char_x, float char_y, float char_z);
	void toggleIntroState();
	void launchVictoryState();
	void showLoadingScreen();
	void loadLevel(const char* level_name);
	void saveLevel();
	void clearSaveData();
	void loadEntities(const char* file_name);
	void resume();
	void exit();

	void test(const char* to_print);
};

#endif
