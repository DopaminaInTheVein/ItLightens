#ifndef INC_TRACKER_H_
#define INC_TRACKER_H_

/*
#define MAX_TRACK_POINTS 1000

#include "comp_msgs.h"
#include "comp_base.h"
#include "comp_physics.h"
#include "comp_transform.h"

struct HandleTrack {
  CHandle handle;
  float normalTime;
};
typedef std::vector<HandleTrack> VHandleTracks;

struct TCompTrackerCinta : public TCompBase {
  VHandleTracks followers;
  VEC3 positions[MAX_TRACK_POINTS];
  VEC3 orientations[MAX_TRACK_POINTS];

  int size;
  float longitude;
  float mSpeed = 1.f;
  float normalSpeed;

  //float	mEpsilon = 0.5f;	//distance margin
  //float	mSpeed;
  //VEC3	mMovement;
  //VEC3	mOrigin;
  //VEC3	mEnd;
  //int		mDirection = 1;

  void onCreate(const TMsgEntityCreated&);
  void setFollower(const TMsgFollow &msg);
  void update(float elapsed);
  void updateTrackMovement(HandleTrack& ht);
  bool load(MKeyValue& atts);
  void renderInMenu();

  VEC3 evaluatePos(HandleTrack ht);
};
*/
#endif
