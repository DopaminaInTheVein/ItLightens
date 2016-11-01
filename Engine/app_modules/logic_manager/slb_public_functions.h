#ifndef INC_SLB_PUBLIC_FUNCTIONS_H_
#define	INC_SLB_PUBLIC_FUNCTIONS_H_

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "logic_manager.h"
#include "app_modules/sound_manager/sound_manager.h"
#include "app_modules/lang_manager/lang_manager.h"
#include "handle/handle.h"
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
	void unPossess();
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
	SLBHandle() {}
	SLBHandle(CHandle h, std::string name, std::string tag = "")
		: real_handle(h), handle_name(name), handle_tag(tag) {}
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
	void setSize(float size);
	void setVisible(int visible);
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
	int addOption(const char* name);
	void selectOption(int);
	bool isPatrolling();
	bool isComeBack();
	void setGuiEnabled(bool);
	void setDragValue(float);

	//Handle Particles
	void SLBHandle::particlesOn();
	void SLBHandle::particlesOff();
	void SLBHandle::particlesLoop();
	void SLBHandle::particlesLoad(const char* name, int enabled);
};

class SLBHandleGroup
{
	VHandles handle_group;

public:
	void getHandlesByTag(const char * tag);
	void awake();
	void removePhysics();
	void destroy();
	void setVisible(int visible);
};

// camera control in LUA
class SLBCamera
{
	CHandle camera_h;
	void execCinematic(const char* name, float speed, bool is_start);
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
	void startCinematic(const char* name, float speed = 0.f);
	void skipCinematic();
	void fadeIn(float speed = 0.f);
	void fadeOut(float speed = 0.f);
	void setFogHeight(float h);
	void fogFade(float x, float y, float z, float hprev, float hnext, float rmin, float rmax);
	void fogUnfade();
	void orbit(bool);
	void resetCamera();
	void fx(const char * name, int enabled);
	void startVibration(float x_max, float y_max, float speed);
	void stopVibration(float speed);
};

// camera control in LUA
class SLBUiCamera
{
	CHandle ui_camera_h;

public:

	// camera functions
	void getCamera();
	bool checkCamera();
	void fadeIn(float speed = 0.f);
	void fadeOut(float speed = 0.f);
	void fx(const char * name, bool enabled);
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
	void waitButton(const char* exec_code);
	void waitEscape(const char* exec_code);
	void waitAction(const char* exec_code);
	void cancelWaitButton();
	void cancelWaitEscape();
	void completeTasklist(int i);
	void print(const char* to_print);
	void breakpoint(int n);
	void setupGame();
	void setLanguage(const char*);
	void setControlEnabled(int);
	void setPlayerEnabled(int);
	void setCameraEnabled(int);
	void setOnlySense(int);

	void playSound(const char* sound_route, float volume, bool looping);
	void play3dSound(const char* sound_route, float s_x, float s_y, float s_z, float max_volume, bool looping, int max_instances);
	void playFixed3dSound(const char* sound_route, const char* sound_name, float s_x, float s_y, float s_z, float max_volume, bool looping);
	void playMusic(const char* music_route, float volume);
	void playVoice(const char* voice_route);
	void playAmbient(const char* ambient_route);

	void stopSound(const char* sound_route);
	void stopFixedSound(const char* sound_name);
	void stopAllSounds();
	void stopMusic();

	void setMusicVolume(float volume);
	void setSFXVolume(float volume);

	void playVideo(const char* video_route);
	void playVideoAndDo(const char* video_route, const char* lua_code);
	void playerRoom(int newRoom);
	void playerTalks(const char* text);
	void showMessage(const char* text, const char* icon);
	void hideMessage();
	void putText(const char* id, const char* text, float posx, float posy, const char* textColor, float scale, const char* textColorTarget, float textColorSpeed, float textColorSpeedLag);
	void putTextUi(const char* id, const char* text, float posx, float posy, const char* textColor, float scale);
	//AI control
	void AIStart();
	void AIStop();
	//void alterText(const char* id, float new_posx, float new_posy, float new_scale);
	void removeText(const char* id);
	SLBHandle characterGlobe(const char* route, float distance, float char_x, float char_y, float char_z, float ttl, float max_distance);
	void addAimCircle(const char* id, const char* prefab, float char_x, float char_y, float char_z, float ttl);
	void removeAimCircle(const char* id);
	void toggleIntroState();
	void launchVictoryState();
	//void showLoadingScreen();
	void forceSenseVision();
	void unforceSenseVision();
	SLBHandle SLBPublicFunctions::create(const char* name, float x, float y, float z);
	void loadLevel(const char* level_name);
	void clearLevel();
	void saveLevel();
	void clearSaveData();
	void loadEntities(const char* file_name);
	void resume();
	void jsonEdit(std::string filename, std::string group, std::string name, float new_value);
	float jsonRead(std::string filename, std::string group, std::string name);
	void jsonEditStr(std::string filename, std::string group, std::string name, std::string new_value);
	std::string jsonReadStr(std::string filename, std::string group, std::string name);
	void pauseGame();
	void resumeGame();
	void setCursorEnabled(bool);
	void exit();
	const char* getText(const char* scene, const char* event);

	void test(const char* to_print);
};

#endif
